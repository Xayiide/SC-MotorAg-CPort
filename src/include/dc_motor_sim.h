#ifndef DC_MOTOR_SIM_H_
#define DC_MOTOR_SIM_H_

struct dcmsim_info {
    float motor_t;

    float w_ec_w_factor;
    float w_ec_i_factor;
    float i_ec_v_factor;
    float i_ec_i_factor;

    float R, L, J, fv, K, ea;

    float i[2];
    float w[2];
};

void dcmsim_init(struct dcmsim_info *i);
void dcmsim_set_ea(struct dcmsim_info *i, float new_ea);
float dcmsim_get_speed(struct dcmsim_info *i);
void dcmsim_exec_sim_cycle(struct dcmsim_info *i);


#endif /* DC_MOTOR_SIM_H_ */