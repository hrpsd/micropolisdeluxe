// This file is part of Micropolis-SDL2PP
// Micropolis-SDL2PP is based on Micropolis
//
// Copyright © 2022 - 2024 Leeor Dicker
// Copyright © 2025 - 2026 Sylvain Nowé
//
// Portions Copyright © 1989-2007 Electronic Arts Inc.
//
// Micropolis-SDL2PP is free software; you can redistribute it and/or modify
// it under the terms of the GNU GPLv3, with additional terms. See the README
// file, included in this distribution, for details.
#pragma once

#include "Point.h"
#include "w_resrc.h"
#include <queue>

class Budget;

int SendMes(NotificationId, MPoint<int> location = { -1, -1 });
void SendMessages(const Budget&);
void ClearMes();
void SendMesAt(NotificationId, int x, int y);
void doMessage();

NotificationId MessageId();
void MessageId(NotificationId, MPoint<int> location = { -1, -1 });

void MessageLocation(MPoint<int> location);
const MPoint<int>& MessageLocation();
void AutoGotoMessageLocation(bool autogo);
bool AutoGotoMessageLocation();
std::deque<std::string> &LastMessages();
void MessageDisplayTime(int time);
int MessageDisplayTime();
void doMessage();
