--create the work library
vlib work
vmap work work

--compile the Verilog and SystemVerilog files
vlog -reportprogress 300 -work work {CONTROL.sv}
vlog -reportprogress 300 -work work {DMEMORY.sv}
vlog -reportprogress 300 -work work {EXECUTE.sv}
vlog -reportprogress 300 -work work {IDECODE.sv}
vlog -reportprogress 300 -work work {IFETCH.sv}
vlog -reportprogress 300 -work work {MIPS.sv}
vlog -reportprogress 300 -work work {STALL_CONT.sv}
vlog -reportprogress 300 -work work {WRITE_BACK.sv}


--compile the testbed
vlog -reportprogress 300 -work work {MIPS_tb.sv}


--Run the simulation
vsim work.MIPS_pipelined_tb
--do wave.do

--run the simulation
run 100us
