#include "Animation.h"

#define RUN "ThirdPersonRun.anim"
#define WALK "ThirdPersonWalk.anim"

#pragma region Animation
void Animation::Init()
{
    for (unsigned int i = 0; i < GetSkeletonBoneCount() - 7; ++i)
    {
        const char* name = GetSkeletonBoneName(i);
        int ParentIndex = GetSkeletonBoneParentIndex(i);

        //IF IT CRASHES HERE, RESTART THE PROGRAM
        const char* parentName = GetSkeletonBoneName(ParentIndex);
        Bone* bone = new Bone(name, i);

        Bone* parent = GetBoneByID(ParentIndex);
        if (parent != nullptr)
        {
            bone->parent = parent;
            bone->parent->childs.push_back(bone);
        }
        float x, y, z;
        float qx, qy, qz, qw;
        GetSkeletonBoneLocalBindTransform(bone->id, x, y, z, qw, qx, qy, qz);
        bone->TLocalPosition = Vector4F(x, y, z, 1);
        bone->TLocalRotation = Quaternion(qx, qy, qz, qw);

        m_bones.push_back(bone);
    }

    //GetAnimLocalBoneTransform(RUN, id, key % GetAnimKeyCount(RUN), x, y, z, qw, qx, qy, qz);
    for (unsigned int i = 0; i < GetSkeletonBoneCount(); ++i)
    {

        AnimData data;
        for (unsigned int k = 0; k < GetAnimKeyCount(RUN); ++k)
        {
            Vector3F position;
            Quaternion quaternion;
            float w, x, y, z;

            GetAnimLocalBoneTransform(RUN, i, k, position.x, position.y, position.z, w, x, y, z);

            quaternion.axis.x = x;
            quaternion.axis.y = y;
            quaternion.axis.z = z;
            quaternion.w = w;

            data.EmplaceVector(position);
            data.EmplaceQuaternion(quaternion);

        }
        GetAnimData().push_back(data);
    }

    for (unsigned int i = 0; i < m_bones.size(); ++i)
    {
        if (m_bones[i]->parent == nullptr)
        {
            if (i == 0)
            {
                m_bones[i]->parent = nullptr;
                continue;
            }

            int ParentID = GetSkeletonBoneParentIndex(m_bones[i]->id);
            if (ParentID < 0)
                continue;

            m_bones[i]->parent = GetBoneByID(ParentID);
            m_bones[i]->parent->childs.push_back(m_bones[i]);
        }
    }
    m_bones[0]->SetTransform();
}

void Animation::Update(float frameTime)
{
    timer += frameTime * 20;
    m_bones[0]->Animate((int)timer, (timer - (int)timer), GetAnimData());

    for (unsigned int i = 1; i < m_bones.size(); ++i)
    {
        for (Bone* child : m_bones[i]->childs)
        {
            DrawLine(child->AnimatedTransform[3], child->AnimatedTransform[7], child->AnimatedTransform[11],
                m_bones[i]->AnimatedTransform[3], m_bones[i]->AnimatedTransform[7], m_bones[i]->AnimatedTransform[11], 1, 0, 0);
        }
    }
    SendToShader();
}

void Animation::SendToShader()
{
    float data[976];

    for (Bone* bone : m_bones)
        EmplaceData(bone, data, bone->id);

    SetSkinningPose(data, GetSkeletonBoneCount() - 7);
}

AnimData& Animation::GetAnimationData(int id)
{
    return GetAnimData()[id];
}

Bone* Animation::GetBoneByID(int index)
{
    for (unsigned int i = 0; i < m_bones.size(); ++i)
    {
        if (index == m_bones[i]->id)
            return m_bones[i];
    }

    return nullptr;
}

void Animation::EmplaceData(Bone* bone, float* data, int i)
{
    for (int k = 0; k < 16; ++k)
        data[(i * 16) + k] = bone->GetData()[k];
}
#pragma endregion

#pragma region Bone
float* Bone::GetData()
{
    Matrix4F data = AnimatedTransform * Matrix4F::Inverse(Tpose);
    data.Normalize();
    return data.m_data;
}

void Bone::Animate(int key, const float lerpFactor, std::vector<AnimData>& animationData)
{
    Vector3F position1;
    Vector3F position2;
    Quaternion Quat1;
    Quaternion Quat2;

    position1 = animationData[id].GetVector(key % GetAnimKeyCount(RUN));
    position2 = animationData[id].GetVector((key + 1) % GetAnimKeyCount(RUN));

    Quat1 = animationData[id].GetQuaternion(key % GetAnimKeyCount(RUN));
    Quat2 = animationData[id].GetQuaternion((key + 1) % GetAnimKeyCount(RUN));

    Matrix4F translate = Matrix4F::CreateTranslation(Vector3F::Lerp(position1, position2, lerpFactor));
    Matrix4F quat = Quaternion::SlerpShortestPath(Quat1, Quat2, lerpFactor).ToMatrix4();

    LocalCurrentMatrix = translate * quat;

    Matrix4F FinalMatrix;

    if (parent != nullptr)
    {
        GlobalMatrix = (LocalTransform * LocalCurrentMatrix);
        FinalMatrix = parent->AnimatedTransform * GlobalMatrix;
    }
    else
    {
        GlobalMatrix = (LocalTransform * LocalCurrentMatrix);
        FinalMatrix = GlobalMatrix;
    }

    AnimatedTransform = FinalMatrix;

    for (Bone* child : childs)
    {
        child->Animate(key, lerpFactor, animationData);
    }
}

void Bone::SetTransform()
{
    Matrix4F translate = Matrix4F::CreateTranslation(Vector3F(TLocalPosition.x, TLocalPosition.y, TLocalPosition.z));
    Matrix4F quat = TLocalRotation.ToMatrix4();
    Matrix4F scale = Matrix4F::CreateScale(Vector3F(1, 1, 1));
    LocalTransform = translate * quat;

    if (parent != nullptr)
        Tpose = parent->Tpose * LocalTransform;
    else
        Tpose = LocalTransform;

    for (Bone* child : childs)
    {
        child->SetTransform();
    }
}
#pragma endregion