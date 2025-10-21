class IBase
{
public:
    virtual void moveBase() = 0;
};

class IArm
{
public:
    virtual void moveArm() = 0;
};

class ICamera
{
public:
    virtual void readCamera() = 0;
};

// ISP principle. Note that mobile base never has to define anything about the arm
class MobileBase: public IBase {};

class MobileManipulator: public IBase, IArm {};

class MobileRobot: public IBase, IArm, ICamera {};

