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
#include "s_msg.h"

#include "main.h"

#include "Budget.h"
#include "Evaluation.h"

#include "s_sim.h"

#include "w_resrc.h"
#include "w_sound.h"
#include "w_tk.h"
#include "w_util.h"

#include "Point.h"

#include <algorithm>
#include <string>
#include <queue>


namespace
{
    int LastCityPop{};
    int LastPictureId{};
    int lastMessageTime{};

    NotificationId messageId{};
    NotificationId LastCategory{};

    bool AutoGotoLocation{ false };

    MPoint<int> messageLocation{};
    std::deque<std::string> lastMessages;

    constexpr auto DefaultMessageDisplayTime{ 3000 };

    int messageDisplayTime{ DefaultMessageDisplayTime };

    int TickCount()
    {
        return static_cast<int>(SDL_GetTicks());
    }

};


void MessageDisplayTime(int time)
{
    messageDisplayTime = time;
}


int MessageDisplayTime()
{
    return messageDisplayTime;
}


void AutoGotoMessageLocation(bool autogo)
{
    AutoGotoLocation = autogo;
}


bool AutoGotoMessageLocation()
{
    return AutoGotoLocation;
}


void LastMessage(const std::string& message)
{
	printf("LAST MESSAGE: \"%s\"\n", message.c_str());
	
	lastMessages.push_front(message);
	
	while (lastMessages.size() > 8)
	{
		lastMessages.pop_back();
	}
}


std::deque<std::string>& LastMessages()
{
    return lastMessages;
}


void LastMessageTime(int tick)
{
    lastMessageTime = tick;
}


int LastMessageTime()
{
    return lastMessageTime;
}


NotificationId MessageId()
{
    return messageId;
}


void MessageId(NotificationId id, MPoint<int> location)
{
    printf("MessageId %d\n", (int)id);
    messageId = id;
    MessageLocation(location);
    doMessage();
}


void MessageLocation(MPoint<int> location)
{
    messageLocation = location;
}


const MPoint<int>& MessageLocation()
{
    return messageLocation;
}


void ClearMes()
{
    MessageId(NotificationId::None, MPoint<int>{ -1, -1 });
    MessageLocation({ -1, -1 });
    LastPictureId = 0;
    LastMessageTime(0);
    //LastMessage("");
}


int SendMes(NotificationId id, MPoint<int> location)
{
    MessageId(id, location);

    if (id == NotificationId::None)
    {
        ClearMes();
    }

    LastMessageTime(TickCount());

    return 1;
}


void SendMesAt(NotificationId id, int x, int y)
{
    SendMes(id, MPoint<int>{ x, y });
}


void SetMessageField(const std::string& msg)
{
    printf("SetMessageField \"%s\"\n", msg.c_str());
    if (msg != "" && (LastMessages().empty() || LastMessages().front() != msg))
    {
        LastMessage(msg);
		
		if ((int)MessageId() > 8)
		{
			showPopup(-(int)MessageId());
		}
    }
}


void DoAutoGoto(int x, int y, const std::string& msg)
{
    mustGoto(x, y);
    
    SetMessageField(msg);
    Eval(std::string("UIAutoGoto " + std::to_string(x) + " " + std::to_string(y)).c_str());
}


void DoShowPicture(int id)
{
    Eval(std::string("UIShowPicture " + std::to_string(id)).c_str());
}


void DoLoseGame()
{
    Eval("UILoseGame");
}


void DoWinGame()
{
    Eval("UIWinGame");
}


void DoScenarioScore(int type)
{
    int z;

    z = -200;	/* you lose */
    switch (type)
    {
    case 1:	/* Dullsville */
        if (cityClass() >= CityClass::Metropolis)
        {
            z = -100;
        }
        break;

    case 2:	/* San Francisco */
        if (cityClass() >= CityClass::Metropolis)
        {
            z = -100;
        }
        break;

    case 3:	/* Hamburg */
        if (cityClass() >= CityClass::Metropolis)
        {
            z = -100;
        }
        break;

    case 4:	/* Bern */
        if (trafficAverage() < 80)
        {
            z = -100;
        }
        break;

    case 5:	/* Tokyo */
        if (cityScore() > 500)
        {
            z = -100;
        }
        break;

    case 6:	/* Detroit */
        if (CrimeAverage < 60)
        {
            z = -100;
        }
        break;

    case 7:	/* Boston */
        if (cityScore() > 500)
        {
            z = -100;
        }
        break;

    case 8:	/* Rio de Janeiro */
        if (cityScore() > 500)
        {
            z = -100;
        }
        break;
    }

    ClearMes();
    //SendMes(z);

    if (z == -200)
    {
        DoLoseGame();
    }
}


void CheckGrowth()
{
    if (CityTime % 4 != 0)
    {
        return;
    }

    int currentPopulation = ((ResPop)+(ComPop * 8) + (IndPop * 8)) * 20;
    NotificationId growthMessageId = NotificationId::None;

    if (LastCityPop)
    {
        if ((LastCityPop < 2000) && (currentPopulation >= 2000))
        {
            growthMessageId = NotificationId::ReachedTown;
        }
        if ((LastCityPop < 10000) && (currentPopulation >= 10000))
        {
            growthMessageId = NotificationId::ReachedCity;
        }
        if ((LastCityPop < 50000L) && (currentPopulation >= 50000L))
        {
            growthMessageId = NotificationId::ReachedCapital;
        }
        if ((LastCityPop < 100000L) && (currentPopulation >= 100000L))
        {
            growthMessageId = NotificationId::ReachedMetropolis;
        }
        if ((LastCityPop < 500000L) && (currentPopulation >= 500000L))
        {
            growthMessageId = NotificationId::ReachedMegalopolis;
        }
    }
    if (growthMessageId != NotificationId::None &&
        growthMessageId != LastCategory)
    {
        SendMes(growthMessageId);
        LastCategory = growthMessageId;
    }

    LastCityPop = currentPopulation;

}


void SendMessages(const Budget& budget)
{
    if ((ScenarioID) && (ScoreType) && (ScoreWait))
    {
        ScoreWait--;
        if (!ScoreWait)
        {
            DoScenarioScore(ScoreType);
        }
    }

    CheckGrowth();

    TotalZPop = ResZPop + ComZPop + IndZPop;
    int PowerPop = NuclearPop + CoalPop;

    switch (CityTime % 64)
    {

    case 1:
        if ((TotalZPop / 4) >= ResZPop) /* need Res */
        {
            SendMes(NotificationId::ResidentialNeeded);
        }
        break;

    case 5:
        if ((TotalZPop / 8) >= ComZPop) /* need Com */
        {
            SendMes(NotificationId::CommercialNeeded);
        }
        break;

    case 10:
        if ((TotalZPop / 8) >= IndZPop) /* need Ind */
        {
            SendMes(NotificationId::IndustrialNeeded);
        }
        break;

    case 14:
        if ((TotalZPop > 10) && ((TotalZPop << 1) > RoadTotal))
        {
            SendMes(NotificationId::RoadsNeeded);
        }
        break;

    case 18:
        if ((TotalZPop > 50) && (TotalZPop > RailTotal))
        {
            SendMes(NotificationId::RailNeeded);
        }
        break;

    case 22:
        if ((TotalZPop > 10) && (PowerPop == 0)) /* need Power */
        {
            SendMes(NotificationId::PowerNeeded);
        }
        break;

    case 26:
        if ((ResPop > 500) && (StadiumPop == 0)) /* need Stad */
        {
            SendMes(NotificationId::StadiumNeeded);
            ResCap = 1;
        }
        else
        {
            ResCap = 0;
        }
        break;

    case 28:
        if ((IndPop > 70) && (PortPop == 0))
        {
            SendMes(NotificationId::SeaportNeeded);
            IndCap = 1;
        }
        else IndCap = 0;
        break;

    case 30:
        if ((ComPop > 100) && (APortPop == 0))
        {
            SendMes(NotificationId::AirportNeeded);
            ComCap = 1;
        }
        else ComCap = 0;
        break;

    case 32: /* dec score for unpowered zones */
    {
        float TM = static_cast<float>(UnpoweredZoneCount + PoweredZoneCount);

        if (TM)
        {
            if ((PoweredZoneCount / TM) < .7)
            {
                SendMes(NotificationId::BlackoutsReported);
            }
        }
    }
        break;

    case 35:
        if (PolluteAverage > 80 /*60*/)
        {
            SendMes(NotificationId::PollutionHigh);
        }
        break;

    case 42:
        if (CrimeAverage > 100)
        {
            SendMes(NotificationId::CrimeHigh);
        }
        break;

    case 45:
        if ((TotalPop > 60) && (FireStPop == 0))
        {
            SendMes(NotificationId::FireDepartmentNeeded);
        }
        break;

    case 48:
        if ((TotalPop > 60) && (PolicePop == 0))
        {
            SendMes(NotificationId::PoliceDepartmentNeeded);
        }
        break;

    case 51:
        if (budget.TaxRate() > 12)
        {
            SendMes(NotificationId::TaxesHigh);
        }
        break;

    case 54:
        if ((RoadEffect < 20) && (RoadTotal > 30))
        {
            SendMes(NotificationId::RoadsDeteriorating);
        }
        break;

    case 57:
        if ((FireEffect < 700) && (TotalPop > 20))
        {
            SendMes(NotificationId::FireDefunded);
        }
        break;

    case 60:
        if ((PoliceEffect < 700) && (TotalPop > 20))
        {
            SendMes(NotificationId::PoliceDefunded);
        }
        break;

    case 63:
        if (trafficAverage() > 60)
        {
            SendMes(NotificationId::TrafficJamsReported);
        }
        break;
    }
}


void doMessage()
{
    bool firstTime = true;
    
    if (MessageId() == NotificationId::None)
    {
        return;
    }
    else if (MessageId() != NotificationId::None &&
             TickCount() - LastMessageTime() > messageDisplayTime)
    {
        //ClearMes();
        //return;
    }

    if (firstTime)
    {
        switch (MessageId())
        {
        case NotificationId::TrafficJamsReported:
            if (RandomRange(0, 5) == 1)
            {
                MakeSound("city", "HonkHonk-Med");
            }
            else if (RandomRange(0, 5) == 1)
            {
                MakeSound("city", "HonkHonk-Low");
            }
            else if (RandomRange(0, 5) == 1)
            {
                MakeSound("city", "HonkHonk-High");
            }
            break;

        case NotificationId::CrimeHigh:
        case NotificationId::FireReported:
        case NotificationId::TornadoReported:
        case NotificationId::EarthquakeReported:
        case NotificationId::PlaneCrashed:
        case NotificationId::ShipWrecked:
        case NotificationId::TrainCrashed:
        case NotificationId::HelicopterCrashed:
            MakeSound("city", "Siren");
            break;

        case NotificationId::MonsterReported:
            //MakeSound("city", "Monster -speed [MonsterSpeed]");
			MakeSound("city", "Monster");
            break;

        case NotificationId::FirebombingReported:
            MakeSound("city", "Explosion-Low");
            MakeSound("city", "Siren");
            break;

        case  NotificationId::NuclearMeltdownReported:
            MakeSound("city", "Explosion-High");
            MakeSound("city", "Explosion-Low");
            MakeSound("city", "Siren");
            break;

        case  NotificationId::RiotsReported:
            MakeSound("city", "Siren");
            break;
                
            default:
                break;
        }

        firstTime = false;
    }

    if (MessageId() != NotificationId::None)
    {
        if (MessageLocation() != MPoint<int>{-1, -1})
        {
            // TODO: draw goto button
        }

        if (AutoGotoMessageLocation() && (MessageLocation() != MPoint<int>{0, 0}))
        {
            DoAutoGoto(MessageLocation().x, MessageLocation().y, NotificationString(MessageId()));
            MessageLocation({ -1, -1 });
        }
        else
        {
            SetMessageField(NotificationString(MessageId()));
        }
    }
    else
    {
        /*
        // picture message
        int pictId = -(MesNum);

        DoShowPicture(pictId);

        MessagePort = pictId; // resend text message

        if (AutoGo && (MesX || MesY))
        {
            DoAutoGoto(MesX, MesY, NotificationString(pictId));
            MesX = 0;
            MesY = 0;
        }
        */
    }
}
