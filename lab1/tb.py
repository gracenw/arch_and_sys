import cocotb
from cocotb.triggers import Timer, ClockCycles, FallingEdge, RisingEdge
from cocotb.clock import Clock

@cocotb.test()
def test_adder(dut):
    #cocotb is lazy and event driven. We can give cocotb
    #a list of events to do, but it won't do them
    #until we request to see Cocotb's state at
    #some point in time.

    #below, we tell cocotb there's an event
    #that repeats every nanosecond, namely
    #the ticking of clock
    cocotb.fork(Clock(dut.clk,1,'ns').start())

    #we set reset high
    dut.reset = 1;

    #simulation currently at time zero
    #we force cocotb to adance simulation until
    #clock has ticked exactly onnce
    yield cocotb.triggers.ClockCycles(dut.clk, 1, 'ns')

    dut.reset = 0;
    dut.a = 4
    dut.b = 5

    #Timer() processes any queued events until the
    #specified time expires
    #Since we forked a tick process above with a clock
    #period of 1ns, advancing the sim clock 1ns below
    #with Timer has the same effect as waiting for clock
    #to tick once
    yield Timer(1,'ns')

    #at this point, dut.sum should be 3
    #we can have python test this for us
    #as well as write it to our log
    assert(dut.sum == 9)
    dut._log.info("SUCCESS!!")
