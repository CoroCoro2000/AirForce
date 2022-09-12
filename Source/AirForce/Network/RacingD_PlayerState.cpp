// Fill out your copyright notice in the Description page of Project Settings.


#include "RacingD_PlayerState.h"
#include "Net/UnrealNetwork.h"

ARacingD_PlayerState::ARacingD_PlayerState()
    : m_ServerRole(EServerRole::NONE)
{
    
}

void ARacingD_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ARacingD_PlayerState, m_ServerRole);
}

void ARacingD_PlayerState::SetServerRole(const EServerRole& _serverRole)
{
    m_ServerRole = _serverRole;
}

EServerRole ARacingD_PlayerState::GetServerRole_Implementation() const
{
    return m_ServerRole;
}
