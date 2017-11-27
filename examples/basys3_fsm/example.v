module example(
    input clk,
    input button,
    input dummy,
    input [3:0] harpoon,
    output reg [3:0] led);

    // states
    parameter S0 = 0, S1 = 1, S2 = 2, S3 = 3;

    // state register
    (* fsm_obfuscate = "true" *)
    (* fsm_obfuscate_states = "7" *)
    reg     [1:0]state;
    reg     [1:0]next_state = S0;
    
    // debouncer for button
    reg pb_state;
    wire pb_up;
    reg pb_sync_0;  always @(posedge clk) pb_sync_0 <= ~button;
    reg pb_sync_1;  always @(posedge clk) pb_sync_1 <= pb_sync_0;
    
    reg [15:0] pb_cnt;
    wire pb_idle = (pb_state==pb_sync_1);
    wire pb_cnt_max = &pb_cnt;
    
    always @(posedge clk)
    if(pb_idle)
        pb_cnt <= 0;
    else
    begin
        pb_cnt <= pb_cnt + 16'd1;
        if(pb_cnt_max) pb_state <= ~pb_state;
    end
    assign pb_up   = ~pb_idle & pb_cnt_max &  pb_state;
    
    
    // fsm
    
    // state register process
    always @ (posedge clk) begin
       state = next_state;
    end
    

    // transitions
    always @* begin
        next_state = S0;
        case (state)
            S0: 
                begin
                    if (pb_up)
                        next_state = S1;
                    else if (dummy)
                        next_state = S2;
                    else
                        next_state = S0;
                end
            S1:
                begin
                    if (pb_up)
                        next_state = S2;
                    else
                        next_state = S1;
                end
            S2:
                begin
                    if (pb_up)
                        next_state = S3;
                    else
                        next_state = S2;
                end
            S3:
                begin
                    if (pb_up)
                        next_state = S0;
                    else
                        next_state = S3;
                end
            default:
                next_state = S0;
        endcase
    end
    
    // outputs
    always @* begin
    led[3:0] = 0;
       case (state)
           S0:
                begin
                    led[0] = 1;
                end
           S1:
                begin
                    led[1] = 1;
                end
           S2:
                begin
                    led[2] = 1;
                end
           S3:
                begin
                    led[3] = 1;
                end
           default:
                led[3:0] = 0;
       endcase
    end
endmodule
