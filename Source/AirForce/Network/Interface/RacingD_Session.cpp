// Fill out your copyright notice in the Description page of Project Settings.


#include "RacingD_Session.h"

// Add default functionality here for any IRacingD_Session functions that are not pure virtual.
EServerRole IRacingD_Session::GetServerRole_Implementation() const
{
    return EServerRole::NONE;
}
