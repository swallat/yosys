//
// Created by Nils Albartus on 17.10.17.
//

#include "kernel/log.h"
#include "kernel/register.h"
#include "kernel/sigtools.h"
#include "kernel/consteval.h"
#include "kernel/celltypes.h"
#include "fsmdata.h"
#include <cmath>
#include <string>
#include <errno.h>
#include <random>
#include <ctime>

USING_YOSYS_NAMESPACE
PRIVATE_NAMESPACE_BEGIN

    template<class InIt>
    std::string print_range(InIt first, InIt last, char const* delim = "\n"){
        std::stringstream string;
        --last;
        for(; first != last; ++first){
            string << *first << delim;
        }
        string << *first;
        return string.str();
    }

    static void fsm_obfuscate(RTLIL::Cell *cell, RTLIL::Module *module, int num_of_obfuscated_states, RTLIL::Wire *harpoon_signal) {
        // load fsm data
        FsmData fsm_data;
        fsm_data.copy_from_cell(cell);
        int number_of_original_states = fsm_data.state_table.size();
        int number_of_total_states = fsm_data.state_table.size() + num_of_obfuscated_states;
        fsm_data.state_bits = number_of_total_states;

        // do recoding of previous states
        int state_idx_counter = fsm_data.reset_state >= 0 ? 1 : 0;
        for (int i = 0; i < fsm_data.state_table.size(); i++)
        {
            int state_idx = fsm_data.reset_state == i ? 0 : state_idx_counter++;
            RTLIL::Const new_code;
            new_code = RTLIL::Const(RTLIL::State::Sa, fsm_data.state_bits);
            new_code.bits[state_idx] = RTLIL::State::S1;

            fsm_data.state_table[i] = new_code;
            log("\t%s\n", fsm_data.state_table[i].as_string().c_str());

        }

        //add new obfuscated states
        for (int i = fsm_data.state_table.size(); i < number_of_total_states; i++){
            int state_idx = fsm_data.reset_state == i ? 0 : state_idx_counter++;
            RTLIL::Const new_code;
            new_code = RTLIL::Const(RTLIL::State::Sa, fsm_data.state_bits);
            new_code.bits[state_idx] = RTLIL::State::S1;

            fsm_data.state_table.push_back(new_code);
            log("\t%s\n", fsm_data.state_table[i].as_string().c_str());
        }



        // transitions
        // add harpoon ctrl signal to ports
        RTLIL::SigSpec sig_in = cell->getPort("\\CTRL_IN");
        int ctrl_in_length_before_harpoon = sig_in.size();
        sig_in.append(harpoon_signal);
        cell->setPort("\\CTRL_IN", sig_in);

        log("\n");
        log("  Input signals:\n");
        sig_in = cell->getPort("\\CTRL_IN");
        for (int i = 0; i < GetSize(sig_in); i++)
            log("  %3d: %s\n", i, log_signal(sig_in[i]));



        int ctrl_in_with_harpoon_sig = ctrl_in_length_before_harpoon + harpoon_signal->width;
        fsm_data.num_inputs = ctrl_in_with_harpoon_sig;


        // recode of old transitions
        for (int i = 0; i < fsm_data.transition_table.size(); i++){
            FsmData::transition_t tr = fsm_data.transition_table[i];
            for (int j = 0; j < harpoon_signal->width; j++){
                tr.ctrl_in.bits.insert(tr.ctrl_in.bits.begin(), RTLIL::State::Sa);
            }
            fsm_data.transition_table[i] = tr;
        }


        // add random transitions
        std::srand((int)std::time(0));

        int number_of_harpoon_bits = harpoon_signal->width;
        log("\tnumber_of_harpoon_bits: %d\n", number_of_harpoon_bits);

        for (int i = number_of_original_states; i < number_of_total_states; i++){
            log("\tstate: %d", i);
            int random_number_of_transitions = (rand() % num_of_obfuscated_states) + 1;
            log(" , number_of transitions: %d\n", random_number_of_transitions);
            for (int j = 0; j < random_number_of_transitions; j++){
                FsmData::transition_t tr;

                // figuring out random jump between two states
                int random_state_out;
                do {
                    random_state_out = (rand() % num_of_obfuscated_states) + number_of_original_states;
                } while (random_state_out == i);

                // original signal has to be don't care
                for (int k = 0; k < ctrl_in_length_before_harpoon; k++){
                    tr.ctrl_in.bits.insert(tr.ctrl_in.bits.begin(), RTLIL::State::Sa);
                }

                // setting random ctrl_inputs
                for (int k = ctrl_in_length_before_harpoon; k < ctrl_in_with_harpoon_sig; k++){
                    int random_value = (rand() % 3);
                    switch(random_value) {
                        case 0: {
                            tr.ctrl_in.bits.insert(tr.ctrl_in.bits.begin(), RTLIL::State::S0);
                            break;
                        }
                        case 1: {
                            tr.ctrl_in.bits.insert(tr.ctrl_in.bits.begin(), RTLIL::State::S1);
                            break;
                        }
                        case 2: {
                            tr.ctrl_in.bits.insert(tr.ctrl_in.bits.begin(), RTLIL::State::Sa);
                            break;
                        }
                        default:
                            break;
                    }
                }

                tr.state_in = i;
                tr.state_out = random_state_out;



                for (int k = 0; k < fsm_data.num_outputs; k++){
                    tr.ctrl_out.bits.push_back(RTLIL::State::Sa);
                }

                log("\ttransition: %d (%s) --> %d (%s)\n", i, log_signal(tr.ctrl_in), random_state_out, log_signal(tr.ctrl_out));

                fsm_data.transition_table.push_back(tr);
            }
        }


        // harpoon key
        std::vector<int> harpoon_key;
        std::vector<int>::iterator it;
        int length_of_key = (rand() % num_of_obfuscated_states) + 1;



        // generate key
        it = harpoon_key.begin();
        for (int i = number_of_original_states; i < number_of_total_states; i++){
            it = harpoon_key.insert ( it , i );
        }
        std::shuffle(harpoon_key.begin(), harpoon_key.end(), std::default_random_engine(std::random_device()()));

        // shorten key
        harpoon_key.erase (harpoon_key.begin()+length_of_key,harpoon_key.end());

        log("\n");
        log("\n\tharpoon key:\n");
        log("\tlength_of_key: %d; harpoon_key: ", length_of_key);
        for (int i : harpoon_key)
            log("%d ", i);
        log("\n");


        for (int i = 0; i < harpoon_key.size(); i++){
            FsmData::transition_t tr;
            // original signal has to be don't care
            for (int k = 0; k < ctrl_in_length_before_harpoon; k++){
                tr.ctrl_in.bits.insert(tr.ctrl_in.bits.begin(), RTLIL::State::Sa);
            }

            // setting random ctrl_inputs
            for (int k = ctrl_in_length_before_harpoon; k < ctrl_in_with_harpoon_sig; k++){
                int random_value = (rand() % 2);
                switch(random_value) {
                    case 0: {
                        tr.ctrl_in.bits.insert(tr.ctrl_in.bits.begin(), RTLIL::State::S0);
                        break;
                    }
                    case 1: {
                        tr.ctrl_in.bits.insert(tr.ctrl_in.bits.begin(), RTLIL::State::S1);
                        break;
                    }
                    default:
                        break;
                }
            }

            tr.state_in = harpoon_key[i];
            if (i == harpoon_key.size() - 1)
                tr.state_out = 0;
            else
                tr.state_out = harpoon_key[i + 1];



            for (int k = 0; k < fsm_data.num_outputs; k++){
                tr.ctrl_out.bits.push_back(RTLIL::State::Sa);
            }

            log("\ttransition: %d (%s) --> %d (%s)\n", tr.state_in, log_signal(tr.ctrl_in), tr.state_out, log_signal(tr.ctrl_out));

            fsm_data.transition_table.push_back(tr);
        }


        // store fsm data
        fsm_data.copy_to_cell(cell);
    }

    struct FsmObfuscatePass : public Pass {
        FsmObfuscatePass() : Pass("fsm_obfuscate", "obfuscate FSM using HARPOON") { }
        virtual void help()
        {
            //   |---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|---v---|
            log("\n");
            log("    fsm_obfuscate [options] [selection]\n");
            log("\n");
            log("This pass obfuscates any FSM automatically using the HARPOON method. \n");
        }
        virtual void execute(std::vector<std::string> args, RTLIL::Design *design)
        {

            log_header(design, "Executing FSM Obfuscation pass.\n");
            size_t argidx;

            for (const auto &mod_it : design->modules_){
                RTLIL::Module *module = mod_it.second;
                int num_of_obfuscated_states = 0;
                int obfuscate_on = 0;

                RTLIL::Wire *harpoon_signal = nullptr;
                bool harpoon_signal_exists = false;
                std::size_t found = false;
                for (const auto &wire_it : module->wires_){
                    found = std::string(wire_it.second->name.c_str()).find("harpoon");
                    if (found != std::string::npos){
                        harpoon_signal_exists = true;
                        harpoon_signal = wire_it.second;
                    }
                }

                for (const auto &cell_it : module->cells_) {

                    RTLIL::Cell *cell = cell_it.second;
                    if (cell_it.second->type == "$fsm" && design->selected(mod_it.second, cell_it.second)){

                        if (harpoon_signal_exists){
                            log("\tharpoon signal found: %s\n", harpoon_signal->name.c_str());
                        }
                        else{
                            log("\tdid you define the harpoon signal in the design?\n");
                        }

                        if (cell->attributes.count("\\fsm_obfuscate_states") > 0){
                            std::string attribute = cell->attributes.at("\\fsm_obfuscate_states").decode_string();
                            num_of_obfuscated_states = std::stoi(attribute);
                        }

                        if (cell->attributes.count("\\fsm_obfuscate") > 0){
                            std::string attribute = cell->attributes.at("\\fsm_obfuscate").decode_string();
                            log("\tname: %s; fsm_obfuscate_attribute: %s\n", cell->name.c_str(), attribute.c_str());
                            std::string true_str = "true";
                            if ((attribute.compare("true") == 0) && (num_of_obfuscated_states > 0)){
                                log("\texecuting fsm obfuscation path with following parameters:\n\tnum_of_states = %d\n", num_of_obfuscated_states);
                                fsm_obfuscate(cell, module, num_of_obfuscated_states, harpoon_signal);
                            } else
                                log("\tcode annotations missing\n");

                        }
                    }
                }

            }
        }
    } FsmObfuscatePass;

PRIVATE_NAMESPACE_END
