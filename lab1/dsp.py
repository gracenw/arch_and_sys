from nmigen import *
from nmigen.back import verilog

class DSP_chain(Elaboratable):
    def __init__ (self,
                    weights,
                    #set the bit-width of our chain
                    WIDTH
                    ):

        #build a parameters dictionary
        self.params = {"weights":weights, "WIDTH":WIDTH}

        #We allow our input and output to be signed
        #incase we were to have an input containing
        #negative weights
        self.x_t = Signal(signed(WIDTH))
        self.y_t = Signal(signed(WIDTH))

    def ports(self):
        return [self.x_t, self.y_t]

    #platform is used to specify FPGA specifics
    #we don't need to use it here beyond making sure
    #the elaborate function signature is correct for
    #nMigen's AST builder when it is called
    def elaborate(self, platform):
        #a module componenent we can return
        m = Module()

        reg_array = []
        for el in range(len(self.params["weights"])):
            #make an array containing registers
            #x[t], x[t - 1], ... x[t - N]
            reg_array += [Signal(
                            signed(self.params["WIDTH"]),
                            name=f"x_t{el}",
                            reset=0)
                            ]

            #when we advance by a clock cycle,
            #we want to pump values as shown below:
            #x[t] = x[t - 1]
            #x[t - 1] = x[t - 2]
            #x[t - 1] = x[t - 2]
            #etc...
            if el > 0:
                #since this is to occur every clock cycle
                #it is a synchronous event. We add the statement
                #to the sync domain

                #in addition, reg_array is laid out in memory
                #such that reg_array[0] = x[t],
                #thus the statement below does implement
                #x[t] = x[t - 1], though it may not seem that way
                m.d.sync += reg_array[el - 1].eq(reg_array[el])

        #grab the last register in the array and set
        #it equal to the input
        #this occurs instantaneously and is never delayed,
        #so we place it under combinational logic
        m.d.comb += reg_array[-1].eq(self.x_t)

        #we use a list comprhrension to mutiply the weights
        #by the values in the respective registers
        #it expands to wieghted_sum = w[0]*x[t] + w[1]+x[t-1] ...
        weighted_sum = sum([el*weight for el,weight in zip(reg_array, self.params["weights"])])
        m.d.comb += self.y_t.eq(weighted_sum)
        return m

#write the verilog
f = open("dsp.v", "w")
top = DSP_chain(WIDTH=8, weights=[1,1,1,1])
f.write(verilog.convert(top,
        name='DSP_chain',
        strip_internal_attrs=True,
        ports=top.ports()
        ))

from nmigen.back.pysim import Simulator, Delay, Settle

def testbench():
    signal_in = [1,1,1,1,1,0]
    signal_out = []

    #pump in all the values from the signal_in
    #list, one value per cycle
    for value in signal_in:
        yield dut.x_t.eq(value)
        #collect the values coming out
        #every cycle
        signal_out += [(yield dut.y_t)]

        #tick the clock by one cycle
        yield

    #run the simulation for 5 more cycles
    #after we finish pumping in values
    for cycle in range(5):
        #collect the values coming out
        #every cycle
        signal_out += [(yield dut.y_t)]

        #tick the clock by one cycle
        yield

    print(signal_out)

#make a Module() the root of our simulation to
#enable top signal to show up in VCDs
m = Module()
dut = top
#we use setattr so that the module is
#named in the VCD
#we could also do m.submodules += dut
#but then the module in the VCD wouldn't be named
setattr(m.submodules,f"dsp_chain",dut)

#instantiate a simulator with 1ns clock
sim = Simulator(m)
sim.add_sync_process(testbench)
sim.add_clock(1/(1e9), domain="sync")

with sim.write_vcd(f"test.vcd", traces=dut.ports()):
	sim.run()
