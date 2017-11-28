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
        sig_in.append(harpoon_signal);
        cell->setPort("\\CTRL_IN", sig_in);

        log("\n");
        log("  Input signals:\n");
        sig_in = cell->getPort("\\CTRL_IN");
        for (int i = 0; i < GetSize(sig_in); i++)
            log("  %3d: %s\n", i, log_signal(sig_in[i]));


        int ctrl_in_length_before_harpoon = sig_in.size();
        int ctrl_in_with_harpoon_sig = ctrl_in_length_before_harpoon + harpoon_signal->width;
        fsm_data.num_inputs = ctrl_in_with_harpoon_sig;

        // recode of old transitions
        for (int i = 0; i < fsm_data.transition_table.size(); i++){
            FsmData::transition_t tr = fsm_data.transition_table[i];
            RTLIL::Const ctrl_in = tr.ctrl_in;
            for (int j = 0; j < harpoon_signal->width; j++){
                ctrl_in.bits.insert(ctrl_in.bits.begin(), RTLIL::State::Sa);
            }
            log("\tctrl_in: %s\n", log_signal(ctrl_in));
            log("\t%s\n", print_range(ctrl_in.bits.rbegin(), ctrl_in.bits.rend(), "").c_str());

            fsm_data.transition_table[i] = tr;
        }


        auto var = cell->parameters.at("\\TRANS_TABLE");
        /*

        // add transitions
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
                    random_state_out = (rand() % (number_of_harpoon_bits)) + number_of_original_states;
                } while (random_state_out == i);
                log("\ttransition: %d --> %d\n", i, random_state_out);

                tr.state_in = i;
                tr.state_out = random_state_out;

                // setting the ctrl_in necessary for the jump
                //how to build control signal?!
                RTLIL::Const ctrl_in;



                tr.ctrl_in = ctrl_in;
                //fsm_data.transition_table.push_back(tr);
            }
        }


        // harpoon key
        std::vector<int> harpoon_key;
        std::vector<int>::iterator it;

        it = harpoon_key.begin();

        // generate key
        for (int i = number_of_original_states; i < number_of_total_states; i++){
            it = harpoon_key.insert ( it , i );
        }
        std::srand((int)std::time(0));
        std::shuffle(harpoon_key.begin(), harpoon_key.end(), std::default_random_engine(std::random_device()()));



        // shorten key
        int length_of_key = 3;

        harpoon_key.erase (harpoon_key.begin()+length_of_key,harpoon_key.end());

        log("\tlength_of_key: %d; harpoon_key: ", length_of_key);
        for (int i : harpoon_key)
            log("%d ", i);
        log("\n");

        // create random transitions
        */

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

            for (auto &mod_it : design->modules_){
                RTLIL::Module *module;
                module = mod_it.second;
                int num_of_obfuscated_states = 0;
                int obfuscate_on = 0;

                RTLIL::Wire *harpoon_signal;
                bool harpoon_signal_exists = false;
                std::size_t found = false;
                for (auto &wire_it : module->wires_){
                    found = std::string(wire_it.second->name.c_str()).find("harpoon");
                    if (found != std::string::npos){
                        harpoon_signal_exists = true;
                        harpoon_signal = wire_it.second;
                    }
                }

                for (auto &cell_it : module->cells_) {

                    RTLIL::Cell *cell;
                    cell = cell_it.second;
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
                                fsm_obfuscate(cell_it.second, mod_it.second, num_of_obfuscated_states, harpoon_signal);
                            } else
                                log("\tcode annotations missing\n");

                        }
                    }
                }

            }
        }
    } FsmObfuscatePass;

PRIVATE_NAMESPACE_END
