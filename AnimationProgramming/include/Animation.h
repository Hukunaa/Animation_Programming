#pragma once 
#include <Simulation.h>
#include <vector>
#include <iostream>
#include <thread>
#include <GPM/GPM.h>

struct AnimData
{
private:
    std::vector<Vector3F> m_animationPositions;
    std::vector<Quaternion> m_animationRotations;

public:

    //Returns the Vector at index "id"
    Vector3F GetVector(int id)
    {
        return m_animationPositions[id];
    }

    //Returns the Quaternion at index "id"
    Quaternion GetQuaternion(int id)
    {
        return m_animationRotations[id];
    }

    void EmplaceVector(Vector3F p_vector) { m_animationPositions.push_back(p_vector); }
    void EmplaceQuaternion(Quaternion p_quaternion) { m_animationRotations.push_back(p_quaternion); }
};

struct Bone
{
    Bone(const char* p_name, int p_id)
    {
        name = p_name;
        id = p_id;
    }


    //Calculate the World Animated Matrix of the Bone and returns it as a 16 float array
    float* GetData();

    //Calculate the Bone Matrices at keyframe "key" in the current animation
    void Animate(int key, const float lerpFactor, std::vector<AnimData>& animationData);

    //Update the bone matrices (only call the function at Init() state)
    void SetTransform();

    GPM::Vector4F TLocalPosition;
    GPM::Quaternion TLocalRotation;


    //Bone Matrices
    GPM::Matrix4F GlobalMatrix;
    GPM::Matrix4F Tpose;
    GPM::Matrix4F LocalTransform;
    GPM::Matrix4F LocalCurrentMatrix;
    GPM::Matrix4F AnimatedTransform;

    //Bone connexions (parent/child)
    Bone* parent = nullptr;
    std::vector<Bone*> childs;

    //Bone informations
    const char* name;
    size_t id;

};

class Animation : public ISimulation
{
public:
    Animation() = default;
    ~Animation() = default;


    float timer{ 0 };

    //Convert Matrix Data from the bone to a float array
    void EmplaceData(Bone* bone, float* data, int i);

    //Send matrices data from all bones to the shader
    void SendToShader();

    virtual void Init() override;
    virtual void Update(float frameTime) override;

    //Try to find the bone at the id "index" and returns it, else null is returned
    AnimData& GetAnimationData(int id);

    //Returns the bone found thanks to its ID, returns null if not found
    Bone* GetBoneByID(int index);


    std::vector<Bone*>& GetBones() { return m_bones; }
    std::vector<AnimData>& GetAnimData() { return m_animData; }

private:

    std::vector<AnimData> m_animData;
    std::vector<Bone*> m_bones;
};
