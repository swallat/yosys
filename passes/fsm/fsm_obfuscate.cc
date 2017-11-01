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
    static void fsm_obfuscate(RTLIL::Design *design, RTLIL::Module *module, RTLIL::Cell *cell, int num_of_obfuscated_states){
        log("Obfuscating state machine from module '%s' and cell '%s'\n", module->name.c_str(), cell->name.c_str());
        log("Adding %d new states to obfuscation\n", num_of_obfuscated_states);
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

                for (auto &cell_it : module->cells_) {
                    RTLIL::Cell *cell;
                    cell = cell_it.second;
                    if (cell_it.second->type == "$fsm"){
                        log("modul: %s, cell: %s\n", module->name.c_str(), cell_it.second->name.c_str());
                        for (auto &att : cell->attributes)
                            log("    attribute: %s, %s\n", att.first.c_str(), att.second.decode_string().c_str());
                        for (auto &att : cell->connections())
                            log("    connections: %s, %s\n", att.first.c_str(), att.second.as_string().c_str());
                    }
                    if (cell->attributes.count("\\fsm_obfuscate") > 0){
                        int num_of_obfuscated_states = std::atoi(cell->attributes.at("\\fsm_obfuscate_states").decode_string().c_str());
                        fsm_obfuscate(design, module, cell, num_of_obfuscated_states);
                    }
                }

            }

        }
    } FsmObfuscatePass;

PRIVATE_NAMESPACE_END
