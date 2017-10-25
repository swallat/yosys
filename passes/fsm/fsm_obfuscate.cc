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
                    if (cell->attributes.count("\\fsm_obfuscate") > 0){
                        std::string attribute = cell->attributes.at("\\fsm_obfuscate").decode_string();
                        log("name: %s; fsm_obfuscate_attribute: %s\n", cell->name.c_str(), attribute.c_str());
                    }

                    if (cell->attributes.count("\\fsm_obfuscate_states") > 0){
                        std::string attribute = cell->attributes.at("\\fsm_obfuscate_states").decode_string();
                        log("fsm_obfuscate_states_attribute: %s\n", attribute.c_str());
                    }
                }

            }

        }
    } FsmObfuscatePass;

PRIVATE_NAMESPACE_END
