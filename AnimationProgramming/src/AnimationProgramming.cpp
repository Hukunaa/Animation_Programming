// AnimationProgramming.cpp : Defines the entry point for the console application.
//
#include <iostream>
#include "stdafx.h"

#include "Engine.h"
#include "Simulation.h"
#include <Animation.h>

int main()
{
    Animation anim;

    //IF IT CRASHES HERE, RESTART THE PROGRAM
	Run(&anim, 1400, 800);

    //IF IT CRASHES HERE, RESTART THE PROGRAM
    system("PAUSE");
    return 0;
}

