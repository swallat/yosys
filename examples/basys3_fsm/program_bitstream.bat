@echo off
SET PATH=C:\Xilinx\Vivado\2017.1\bin;C:\Xilinx\Vivado\2017.1\lib\win64.o;%PATH%
SET XILINX_VIVADO=C:\Xilinx\Vivado\2017.1

vivado -nolog -nojournal -mode batch -source "Z:\Uni\yosys\examples\basys3_fsm\run_prog.tcl"
::vivado -log "Z:\Uni\yosys\examples\basys3_fsm\prog.log" -mode batch -source "Z:\Uni\yosys\examples\basys3_fsm\run_prog.tcl"

pause