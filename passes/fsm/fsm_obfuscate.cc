//
// Created by Nils Albartus on 17.10.17.
//

#include "kernel/log.h"
#include "kernel/register.h"
#include "kernel/sigtools.h"
#include "kernel/consteval.h"
#include "kernel/celltypes.h"
#include "fsmdata.h"
#include <math.h>
#include <string.h>
#include <errno.h>

USING_YOSYS_NAMESPACE
PRIVATE_NAMESPACE_BEGIN

    static void fsm_obfuscate(RTLIL::Cell *cell, RTLIL::Module *module, int num_of_obfuscated_states) {
        // load fsm data
        FsmData fsm_data;
        fsm_data.copy_from_cell(cell);
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


                for (auto &cell_it : module->cells_) {
                    RTLIL::Cell *cell;
                    cell = cell_it.second;
                    if (cell_it.second->type == "$fsm" && design->selected(mod_it.second, cell_it.second)){
                        if (cell->attributes.count("\\fsm_obfuscate_states") > 0){
                            std::string attribute = cell->attributes.at("\\fsm_obfuscate_states").decode_string();
                            num_of_obfuscated_states = std::stoi(attribute);

                        }
                        if (cell->attributes.count("\\fsm_obfuscate") > 0){
                            std::string attribute = cell->attributes.at("\\fsm_obfuscate").decode_string();
                            log("name: %s; fsm_obfuscate_attribute: %s\n", cell->name.c_str(), attribute.c_str());
                            std::string true_str = "true";
                            if ((attribute.compare("true") == 0) && (num_of_obfuscated_states > 0)){
                                log("\texecuting fsm obfuscation path with following parameters:\n\tnum_of_states = %d\n", num_of_obfuscated_states);
                                fsm_obfuscate(cell_it.second, mod_it.second, num_of_obfuscated_states);
                            } else
                                log("\tcode annotations missing\n");

                        }
                    }
                }

            }
        }
    } FsmObfuscatePass;

PRIVATE_NAMESPACE_END
