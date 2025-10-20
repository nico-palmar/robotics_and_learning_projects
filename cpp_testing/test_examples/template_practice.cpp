#include <vector>
#include <iostream>

template<typename T>
class Gripper
{
public:
    Gripper(const float grip_force)
    {
        grip_force_ = grip_force;
    }

    bool grasp_object(const float required_force)
    {
        if (required_force <= grip_force_)
        {
            // simulate some sensor reading
            // simulate appending to joint readings vector
            std::cout << "Grip success" << std::endl;
            return true;
        }
        return false;
    }

private:
    std::vector<T> joint_readings_;
    float grip_force_;
};

template<typename Vector>
float distanceSE2Squared(const Vector& pos_1, const Vector& pos_2, float theta_weight = 1.0)
{
    // should probably have some type checks on the position
    const auto dx = pos_1[0] - pos_2[0];
    const auto dy = pos_1[1] - pos_2[1];
    const auto d_theta = pos_1[2] - pos_2[2];

    return dx*dx + dy*dy + (theta_weight * d_theta)*(theta_weight * d_theta);
}

int main()
{
    std::vector<float> p1 { 10.0, 5.0, 0.1 };
    std::vector<float> p2 { 6.0, 4.0, 0.3 };

    // auto infers the template type
    const auto dist_se2_squared = distanceSE2Squared(p1, p2);
    // const auto dist_se2_squared = distanceSE2Squared<std::vector<float>>(p1, p2);

    std::cout << "Dist se2 sq " << dist_se2_squared << std::endl;
    
    Gripper<float> g1 { 10.0 };

    const auto result = g1.grasp_object(9.0);
}