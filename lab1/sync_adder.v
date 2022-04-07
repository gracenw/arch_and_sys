module adder(sum, a, b, clk, reset);

input wire clk, reset;
input wire [7:0] a;
input wire [7:0] b;
output reg [7:0] sum;

always @(posedge clk)
    if (reset == 1)
        sum <= 0;
    else
        sum <= a+b;

// the "macro" to dump signals
`ifdef COCOTB_SIM
integer num_regs;
initial begin
    $dumpfile ("sim.vcd");
    $dumpvars (0, adder);
end
`endif

endmodule