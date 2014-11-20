/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2006-2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/* ScriptData
SDName: Areatrigger_Scripts
SD%Complete: 100
SDComment: Scripts for areatriggers
SDCategory: Areatrigger
EndScriptData */

/* ContentData
at_coilfang_waterfall           4591
at_legion_teleporter            4560 Teleporter TO Invasion Point: Cataclysm
at_stormwright_shelf            q12741
at_last_rites                   q12019
at_sholazar_waygate             q12548
at_nats_landing                 q11209
at_bring_your_orphan_to         q910 q910 q1800 q1479 q1687 q1558 q10951 q10952
at_brewfest
at_area_52_entrance
EndContentData */

#include "ScriptMgr.h"
#include "Object.h"
#include "ScriptedCreature.h"
#include "AreaTriggerScript.h"

/*######
## at_coilfang_waterfall
######*/

enum eCoilfangGOs
{
    GO_COILFANG_WATERFALL   = 184212
};

class AreaTrigger_at_coilfang_waterfall : public AreaTriggerScript
{
    public:

        AreaTrigger_at_coilfang_waterfall()
            : AreaTriggerScript("at_coilfang_waterfall")
        {
        }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/)
        {
            if (GameObject* go = GetClosestGameObjectWithEntry(player, GO_COILFANG_WATERFALL, 35.0f))
                if (go->getLootState() == GO_READY)
                    go->UseDoorOrButton();

            return false;
        }
};

/*#####
## at_legion_teleporter
#####*/

enum eLegionTeleporter
{
    SPELL_TELE_A_TO         = 37387,
    QUEST_GAINING_ACCESS_A  = 10589,

    SPELL_TELE_H_TO         = 37389,
    QUEST_GAINING_ACCESS_H  = 10604
};

class AreaTrigger_at_legion_teleporter : public AreaTriggerScript
{
    public:

        AreaTrigger_at_legion_teleporter()
            : AreaTriggerScript("at_legion_teleporter")
        {
        }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/)
        {
            if (player->isAlive() && !player->isInCombat())
            {
                if (player->GetTeam() == ALLIANCE && player->GetQuestRewardStatus(QUEST_GAINING_ACCESS_A))
                {
                    player->CastSpell(player, SPELL_TELE_A_TO, false);
                    return true;
                }

                if (player->GetTeam() == HORDE && player->GetQuestRewardStatus(QUEST_GAINING_ACCESS_H))
                {
                    player->CastSpell(player, SPELL_TELE_H_TO, false);
                    return true;
                }

                return false;
            }
            return false;
        }
};

/*######
## at_stormwright_shelf
######*/

enum eStormwrightShelf
{
    QUEST_STRENGTH_OF_THE_TEMPEST               = 12741,

    SPELL_CREATE_TRUE_POWER_OF_THE_TEMPEST      = 53067
};

class AreaTrigger_at_stormwright_shelf : public AreaTriggerScript
{
    public:

        AreaTrigger_at_stormwright_shelf()
            : AreaTriggerScript("at_stormwright_shelf")
        {
        }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/)
        {
            if (!player->isDead() && player->GetQuestStatus(QUEST_STRENGTH_OF_THE_TEMPEST) == QUEST_STATUS_INCOMPLETE)
                player->CastSpell(player, SPELL_CREATE_TRUE_POWER_OF_THE_TEMPEST, false);

            return true;
        }
};

/*######
## at_scent_larkorwi
######*/

enum eScentLarkorwi
{
    QUEST_SCENT_OF_LARKORWI                     = 4291,
    NPC_LARKORWI_MATE                           = 9683
};

class AreaTrigger_at_scent_larkorwi : public AreaTriggerScript
{
    public:

        AreaTrigger_at_scent_larkorwi()
            : AreaTriggerScript("at_scent_larkorwi")
        {
        }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/)
        {
            if (!player->isDead() && player->GetQuestStatus(QUEST_SCENT_OF_LARKORWI) == QUEST_STATUS_INCOMPLETE)
            {
                if (!player->FindNearestCreature(NPC_LARKORWI_MATE, 15))
                    player->SummonCreature(NPC_LARKORWI_MATE, player->GetPositionX()+5, player->GetPositionY(), player->GetPositionZ(), 3.3f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 100000);
            }

            return false;
        }
};

/*#####
## at_last_rites
#####*/

enum eAtLastRites
{
    QUEST_LAST_RITES                          = 12019,
    QUEST_BREAKING_THROUGH                    = 11898,
};

class AreaTrigger_at_last_rites : public AreaTriggerScript
{
    public:

        AreaTrigger_at_last_rites()
            : AreaTriggerScript("at_last_rites")
        {
        }

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger)
        {
            if (!(player->GetQuestStatus(QUEST_LAST_RITES) == QUEST_STATUS_INCOMPLETE ||
                player->GetQuestStatus(QUEST_LAST_RITES) == QUEST_STATUS_COMPLETE ||
                player->GetQuestStatus(QUEST_BREAKING_THROUGH) == QUEST_STATUS_INCOMPLETE ||
                player->GetQuestStatus(QUEST_BREAKING_THROUGH) == QUEST_STATUS_COMPLETE))
                return false;

            WorldLocation pPosition;

            switch (trigger->ID)
            {
                case 5332:
                case 5338:
                    pPosition = WorldLocation(571, 3733.68f, 3563.25f, 290.812f, 3.665192f);
                    break;
                case 5334:
                    pPosition = WorldLocation(571, 3802.38f, 3585.95f, 49.5765f, 0.0f);
                    break;
                case 5340:
                    pPosition = WorldLocation(571, 3687.91f, 3577.28f, 473.342f, 0.0f);
                    break;
                default:
                    return false;
            }

            player->TeleportTo(pPosition);

            return false;
        }
};

/*######
## at_sholazar_waygate
######*/

enum eWaygate
{
    SPELL_SHOLAZAR_TO_UNGORO_TELEPORT           = 52056,
    SPELL_UNGORO_TO_SHOLAZAR_TELEPORT           = 52057,

    AT_SHOLAZAR                                 = 5046,
    AT_UNGORO                                   = 5047,

    QUEST_THE_MAKERS_OVERLOOK                   = 12613,
    QUEST_THE_MAKERS_PERCH                      = 12559,
};

class AreaTrigger_at_sholazar_waygate : public AreaTriggerScript
{
    public:

        AreaTrigger_at_sholazar_waygate()
            : AreaTriggerScript("at_sholazar_waygate")
        {
        }

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger)
        {
            if (player->GetQuestStatus(QUEST_THE_MAKERS_OVERLOOK) == QUEST_STATUS_REWARDED && !player->isDead() &&
                player->GetQuestStatus(QUEST_THE_MAKERS_PERCH)    == QUEST_STATUS_REWARDED)
            {
                switch (trigger->ID)
                {
                    case AT_SHOLAZAR:
                        player->CastSpell(player, SPELL_SHOLAZAR_TO_UNGORO_TELEPORT, false);
                        break;

                    case AT_UNGORO:
                        player->CastSpell(player, SPELL_UNGORO_TO_SHOLAZAR_TELEPORT, false);
                        break;
                }
            }

            return false;
        }
};

/*######
## at_nats_landing
######*/

enum NatsLanding
{
    QUEST_NATS_BARGAIN = 11209,
    SPELL_FISH_PASTE   = 42644,
    NPC_LURKING_SHARK  = 23928
};

class AreaTrigger_at_nats_landing : public AreaTriggerScript
{
    public:
        AreaTrigger_at_nats_landing() : AreaTriggerScript("at_nats_landing") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/)
        {
            if (!player->isAlive() || !player->HasAura(SPELL_FISH_PASTE))
                return false;

            if (player->GetQuestStatus(QUEST_NATS_BARGAIN) == QUEST_STATUS_INCOMPLETE)
            {
                if (!player->FindNearestCreature(NPC_LURKING_SHARK, 20.0f))
                {
                    if (Creature* shark = player->SummonCreature(NPC_LURKING_SHARK, -4246.243f, -3922.356f, -7.488f, 5.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 100000))
                        shark->AI()->AttackStart(player);

                    return false;
                }
            }
            return true;
        }
};

/*######
## at_bring_your_orphan_to
######*/

enum BringYourOrphanTo
{
    QUEST_DOWN_AT_THE_DOCKS         = 910,
    QUEST_GATEWAY_TO_THE_FRONTIER   = 911,
    QUEST_LORDAERON_THRONE_ROOM     = 1800,
    QUEST_BOUGHT_OF_ETERNALS        = 1479,
    QUEST_SPOOKY_LIGHTHOUSE         = 1687,
    QUEST_STONEWROUGHT_DAM          = 1558,
    QUEST_DARK_PORTAL_H             = 10951,
    QUEST_DARK_PORTAL_A             = 10952,

    AT_DOWN_AT_THE_DOCKS            = 3551,
    AT_GATEWAY_TO_THE_FRONTIER      = 3549,
    AT_LORDAERON_THRONE_ROOM        = 3547,
    AT_BOUGHT_OF_ETERNALS           = 3546,
    AT_SPOOKY_LIGHTHOUSE            = 3552,
    AT_STONEWROUGHT_DAM             = 3548,
    AT_DARK_PORTAL                  = 4356,

    AURA_ORPHAN_OUT                 = 58818,
};

class AreaTrigger_at_bring_your_orphan_to : public AreaTriggerScript
{
    public:
        AreaTrigger_at_bring_your_orphan_to() : AreaTriggerScript("at_bring_your_orphan_to") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger)
        {
            uint32 questId = 0;

            if (player->isDead() || !player->HasAura(AURA_ORPHAN_OUT))
                return false;

            switch (trigger->ID)
            {
                case AT_DOWN_AT_THE_DOCKS:
                    questId = QUEST_DOWN_AT_THE_DOCKS;
                    break;
                case AT_GATEWAY_TO_THE_FRONTIER:
                    questId = QUEST_GATEWAY_TO_THE_FRONTIER;
                    break;
                case AT_LORDAERON_THRONE_ROOM:
                    questId = QUEST_LORDAERON_THRONE_ROOM;
                    break;
                case AT_BOUGHT_OF_ETERNALS:
                    questId = QUEST_BOUGHT_OF_ETERNALS;
                    break;
                case AT_SPOOKY_LIGHTHOUSE:
                    questId = QUEST_SPOOKY_LIGHTHOUSE;
                    break;
                case AT_STONEWROUGHT_DAM:
                    questId = QUEST_STONEWROUGHT_DAM;
                    break;
                case AT_DARK_PORTAL:
                    questId = player->GetTeam() == ALLIANCE ? QUEST_DARK_PORTAL_A : QUEST_DARK_PORTAL_H;
                    break;
            }

            if (questId && player->GetQuestStatus(questId) == QUEST_STATUS_INCOMPLETE)
                player->AreaExploredOrEventHappens(questId);

            return true;
        }
};

/*######
## at_brewfest
######*/

enum Brewfest
{
    NPC_TAPPER_SWINDLEKEG       = 24711,
    NPC_IPFELKOFER_IRONKEG      = 24710,

    AT_BREWFEST_DUROTAR         = 4829,
    AT_BREWFEST_DUN_MOROGH      = 4820,

    SAY_WELCOME                 = 4,

    AREATRIGGER_TALK_COOLDOWN   = 5, // in seconds
};

class AreaTrigger_at_brewfest : public AreaTriggerScript
{
    public:
        AreaTrigger_at_brewfest() : AreaTriggerScript("at_brewfest")
        {
            // Initialize for cooldown
            _triggerTimes[AT_BREWFEST_DUROTAR] = _triggerTimes[AT_BREWFEST_DUN_MOROGH] = 0;
        }

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger)
        {
            uint32 triggerId = trigger->ID;
            // Second trigger happened too early after first, skip for now
            if (sWorld->GetGameTime() - _triggerTimes[triggerId] < AREATRIGGER_TALK_COOLDOWN)
                return false;

            switch (triggerId)
            {
                case AT_BREWFEST_DUROTAR:
                    if (Creature* tapper = player->FindNearestCreature(NPC_TAPPER_SWINDLEKEG, 20.0f))
                        tapper->AI()->Talk(SAY_WELCOME, player->GetGUID());
                    break;
                case AT_BREWFEST_DUN_MOROGH:
                    if (Creature* ipfelkofer = player->FindNearestCreature(NPC_IPFELKOFER_IRONKEG, 20.0f))
                        ipfelkofer->AI()->Talk(SAY_WELCOME, player->GetGUID());
                    break;
                default:
                    break;
            }

            _triggerTimes[triggerId] = sWorld->GetGameTime();
            return false;
        }

    private:
        std::map<uint32, time_t> _triggerTimes;
};

/*######
## at_area_52_entrance
######*/

enum Area52Entrance
{
    SPELL_A52_NEURALYZER  = 34400,
    NPC_SPOTLIGHT         = 19913,
    SUMMON_COOLDOWN       = 5,

    AT_AREA_52_SOUTH      = 4472,
    AT_AREA_52_NORTH      = 4466,
    AT_AREA_52_WEST       = 4471,
    AT_AREA_52_EAST       = 4422,
};

class AreaTrigger_at_area_52_entrance : public AreaTriggerScript
{
    public:
        AreaTrigger_at_area_52_entrance() : AreaTriggerScript("at_area_52_entrance")
        {
            _triggerTimes[AT_AREA_52_SOUTH] = _triggerTimes[AT_AREA_52_NORTH] = _triggerTimes[AT_AREA_52_WEST] = _triggerTimes[AT_AREA_52_EAST] = 0;
        }

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger)
        {
            float x = 0.0f, y = 0.0f, z = 0.0f;

            if (!player->isAlive())
                return false;

            uint32 triggerId = trigger->ID;
            if (sWorld->GetGameTime() - _triggerTimes[triggerId] < SUMMON_COOLDOWN)
                return false;

            switch (triggerId)
            {
                case AT_AREA_52_EAST:
                    x = 3044.176f;
                    y = 3610.692f;
                    z = 143.61f;
                    break;
                case AT_AREA_52_NORTH:
                    x = 3114.87f;
                    y = 3687.619f;
                    z = 143.62f;
                    break;
                case AT_AREA_52_WEST:
                    x = 3017.79f;
                    y = 3746.806f;
                    z = 144.27f;
                    break;
                case AT_AREA_52_SOUTH:
                    x = 2950.63f;
                    y = 3719.905f;
                    z = 143.33f;
                    break;
            }

            player->SummonCreature(NPC_SPOTLIGHT, x, y, z, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 5000);
            player->AddAura(SPELL_A52_NEURALYZER, player);
            _triggerTimes[triggerId] = sWorld->GetGameTime();
            return false;
        }

    private:
        std::map<uint32, time_t> _triggerTimes;
};

enum BaelModan
{
    KILL_CREDIT     = 38251,
    SABOTAGE_QUEST  = 24747
};

class AreaTrigger_at_bael_modan : public AreaTriggerScript
{
    public:
        AreaTrigger_at_bael_modan() : AreaTriggerScript("AreaTrigger_at_bael_modan") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger)
        {
            if (player->GetQuestStatus(SABOTAGE_QUEST) == QUEST_STATUS_INCOMPLETE)
                player->KilledMonsterCredit(KILL_CREDIT, 0);

            return true;
        }
};

#define KLAXXI_COUNCIL_QUEST 31006

class AreaTrigger_at_klaxxi_vess : public AreaTriggerScript
{
    public:
        AreaTrigger_at_klaxxi_vess() : AreaTriggerScript("AreaTrigger_at_klaxxi_vess") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger)
        {
            if (player->GetQuestStatus(KLAXXI_COUNCIL_QUEST) == QUEST_STATUS_INCOMPLETE)
                player->KilledMonsterCredit(62538);

            return true;
        }
};

class AreaTrigger_at_farmer_fung : public AreaTriggerScript
{
    public:
        AreaTrigger_at_farmer_fung() : AreaTriggerScript("AreaTrigger_at_farmer_fung") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger)
        {
            if (player->GetQuestStatus(30518) == QUEST_STATUS_INCOMPLETE)
                if (player->IsOnVehicle())
                {
                    if (Creature* creature = player->GetVehicleCreatureBase())
                    {
                        player->KilledMonsterCredit(59491);
                        player->ExitVehicle();
                        creature->DespawnOrUnsummon();
                    }
                }

            return true;
        }
};

class areatrigger_at_serpent_nests : public AreaTriggerScript
{
    public:
        areatrigger_at_serpent_nests() : AreaTriggerScript("areatrigger_at_serpent_nests") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger)
        {
            if (player->GetQuestStatus(30136) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(30157) == QUEST_STATUS_INCOMPLETE)
            {
                std::list<Creature*> serpentsList;
                uint32 entries[3] = { 58220, 58243, 58244 };

                for (uint8 i = 0; i < 3; ++i)
                {
                    GetCreatureListWithEntryInGrid(serpentsList, player, entries[i], 30.0f);

                    for (auto serpent : serpentsList)
                    {
                        if (serpent->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED))
                        {
                            serpent->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED);
                            serpent->RemoveAllAuras();
                            serpent->DespawnOrUnsummon();
                            player->KilledMonsterCredit(58246);
                        }
                    }

                    serpentsList.clear();
                }
            }

            return true;
        }
};

class AreaTrigger_at_mason_s_folly : public AreaTriggerScript
{
    public:
        AreaTrigger_at_mason_s_folly() : AreaTriggerScript("AreaTrigger_at_mason_s_folly") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger)
        {
            if (player->GetQuestStatus(31482) == QUEST_STATUS_INCOMPLETE)
            {
                Creature* creature = GetClosestCreatureWithEntry(player, 64822, 140.0f, true);
                if (!creature)
                {
                    player->KilledMonsterCredit(66586);
                    player->SummonCreature(64822, 755.734f, -507.565f, 442.6f, 4.491428f, TEMPSUMMON_MANUAL_DESPAWN, 0, player->GetGUID());
                }
            }

            return true;
        }
};

class AreaTrigger_ice_trap : public MS::AreaTriggerEntityScript
{
public:
    AreaTrigger_ice_trap()
        : MS::AreaTriggerEntityScript("at_ice_trap")
    {
    }

    MS::AreaTriggerEntityScript* GetAI()
    {
        return new AreaTrigger_ice_trap();
    }

    void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time)
    {
        std::list<Unit*> targetList;
        float l_Radius = 10.0f;
        Unit* l_Caster = p_AreaTrigger->GetCaster();

        JadeCore::NearestAttackableUnitInObjectRangeCheck u_check(p_AreaTrigger, l_Caster, l_Radius);
        JadeCore::UnitListSearcher<JadeCore::NearestAttackableUnitInObjectRangeCheck> searcher(p_AreaTrigger, targetList, u_check);
        p_AreaTrigger->VisitNearbyObject(l_Radius, searcher);

        for (auto itr : targetList)
            itr->CastSpell(itr, 135299, true);

        // Glyph of Black Ice
        if (l_Caster->GetDistance(p_AreaTrigger) <= l_Radius && l_Caster->HasAura(109263) && !l_Caster->HasAura(83559))
            l_Caster->CastSpell(l_Caster, 83559, true);
        else
            l_Caster->RemoveAura(83559);
    }
};

class AreaTrigger_power_word_barrier : public MS::AreaTriggerEntityScript
{
public:
    AreaTrigger_power_word_barrier()
        : MS::AreaTriggerEntityScript("at_power_word_barrier")
    {
    }

    MS::AreaTriggerEntityScript* GetAI()
    {
        return new AreaTrigger_power_word_barrier();
    }

    void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time)
    {
        std::list<Unit*> targetList;
        float l_Radius = 6.0f;

        JadeCore::AnyFriendlyUnitInObjectRangeCheck u_check(p_AreaTrigger, p_AreaTrigger->GetCaster(), l_Radius);
        JadeCore::UnitListSearcher<JadeCore::AnyFriendlyUnitInObjectRangeCheck> searcher(p_AreaTrigger, targetList, u_check);
        p_AreaTrigger->VisitNearbyObject(l_Radius, searcher);

        for (auto itr : targetList)
            itr->CastSpell(itr, 81782, true);
    }
};

class AreaTrigger_ursol_vortex : public MS::AreaTriggerEntityScript
{
public:
    AreaTrigger_ursol_vortex()
        : MS::AreaTriggerEntityScript("at_ursol_vortex")
    {
    }

    MS::AreaTriggerEntityScript* GetAI()
    {
        return new AreaTrigger_ursol_vortex();
    }

    void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time)
    {
        std::list<Unit*> targetList;
        float l_Radius = 8.0f;
        Unit* l_Caster = p_AreaTrigger->GetCaster();

        JadeCore::NearestAttackableUnitInObjectRangeCheck u_check(p_AreaTrigger, l_Caster, l_Radius);
        JadeCore::UnitListSearcher<JadeCore::NearestAttackableUnitInObjectRangeCheck> searcher(p_AreaTrigger, targetList, u_check);
        p_AreaTrigger->VisitNearbyObject(l_Radius, searcher);

        if (!targetList.empty())
        for (auto itr : targetList)
        if (!itr->HasAura(127797))
            l_Caster->CastSpell(itr, 127797, true);
    }
};

class AreaTrigger_healing_sphere : public MS::AreaTriggerEntityScript
{
public:
    AreaTrigger_healing_sphere()
        : MS::AreaTriggerEntityScript("at_healing_sphere")
    {
    }

    MS::AreaTriggerEntityScript* GetAI()
    {
        return new AreaTrigger_healing_sphere();
    }

    void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time)
    {
        std::list<Unit*> targetList;
        float l_Radius = 1.0f;
        Unit* l_Caster = p_AreaTrigger->GetCaster();

        JadeCore::AnyFriendlyUnitInObjectRangeCheck u_check(p_AreaTrigger, l_Caster, l_Radius);
        JadeCore::UnitListSearcher<JadeCore::AnyFriendlyUnitInObjectRangeCheck> searcher(p_AreaTrigger, targetList, u_check);
        p_AreaTrigger->VisitNearbyObject(l_Radius, searcher);

        if (!targetList.empty())
        {
            for (auto itr : targetList)
            {
                l_Caster->CastSpell(itr, 115464, true); // Healing Sphere heal
                p_AreaTrigger->SetDuration(0);
                return;
            }
        }
    }
};

class AreaTrigger_cancel_barrier : public MS::AreaTriggerEntityScript
{
public:
    AreaTrigger_cancel_barrier()
        : MS::AreaTriggerEntityScript("at_cancel_barrier")
    {
    }

    MS::AreaTriggerEntityScript* GetAI()
    {
        return new AreaTrigger_cancel_barrier();
    }

    void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time)
    {
        std::list<Unit*> targetList;
        float l_Radius = 6.0f;

        JadeCore::AnyFriendlyUnitInObjectRangeCheck u_check(p_AreaTrigger, p_AreaTrigger->GetCaster(), l_Radius);
        JadeCore::UnitListSearcher<JadeCore::AnyFriendlyUnitInObjectRangeCheck> searcher(p_AreaTrigger, targetList, u_check);
        p_AreaTrigger->VisitNearbyObject(l_Radius, searcher);

        if (!targetList.empty())
        for (auto itr : targetList)
            itr->CastSpell(itr, 115856, true);
    }
};

class AreaTrigger_rune_of_power : public MS::AreaTriggerEntityScript
{
public:
    AreaTrigger_rune_of_power()
        : MS::AreaTriggerEntityScript("at_rune_of_power")
    {
    }

    MS::AreaTriggerEntityScript* GetAI()
    {
        return new AreaTrigger_rune_of_power();
    }

    void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time)
    {
        std::list<Unit*> targetList;
        float l_Radius = 5.0f;
        Unit* l_Caster = p_AreaTrigger->GetCaster();

        if (l_Caster->IsWithinDistInMap(p_AreaTrigger, 5.0f))
        {
            if (!l_Caster->HasAura(116014))
                l_Caster->CastSpell(l_Caster, 116014, true);
            else if (AuraPtr runeOfPower = l_Caster->GetAura(116014))
                runeOfPower->RefreshDuration();

            if (l_Caster->ToPlayer())
                l_Caster->ToPlayer()->UpdateManaRegen();
        }
    }

    void OnRemove(AreaTrigger* p_AreaTrigger)
    {
        Unit* l_Caster = p_AreaTrigger->GetCaster();
        if (l_Caster && l_Caster->HasAura(116014))
            l_Caster->RemoveAura(116014);
    }
};

class AreaTrigger_amethyst_pool : public MS::AreaTriggerEntityScript
{
public:
    AreaTrigger_amethyst_pool()
        : MS::AreaTriggerEntityScript("at_amethyst_pool")
    {
    }

    MS::AreaTriggerEntityScript* GetAI()
    {
        return new AreaTrigger_amethyst_pool();
    }

    void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time)
    {
        std::list<Unit*> targetList;
        float l_Radius = 5.0f;
        Unit* l_Caster = p_AreaTrigger->GetCaster();

        JadeCore::NearestAttackableUnitInObjectRangeCheck u_check(p_AreaTrigger, l_Caster, l_Radius);
        JadeCore::UnitListSearcher<JadeCore::NearestAttackableUnitInObjectRangeCheck> searcher(p_AreaTrigger, targetList, u_check);
        p_AreaTrigger->VisitNearbyObject(l_Radius, searcher);

        if (!targetList.empty())
        {
            for (auto itr : targetList)
            {
                // Amethyst Pool - Periodic Damage
                if (itr->GetDistance(p_AreaTrigger) > 3.5f && itr->HasAura(130774))
                    itr->RemoveAura(130774);
                else if (itr->GetDistance(p_AreaTrigger) <= 3.5f && !itr->HasAura(130774))
                    l_Caster->CastSpell(itr, 130774, true);
            }
        }
    }
};

class AreaTrigger_cancelling_noise_area_trigger : public MS::AreaTriggerEntityScript
{
public:
    AreaTrigger_cancelling_noise_area_trigger()
        : MS::AreaTriggerEntityScript("at_cancelling_noise_area_trigger")
    {
    }

    MS::AreaTriggerEntityScript* GetAI()
    {
        return new AreaTrigger_cancelling_noise_area_trigger();
    }

    void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time)
    {
        std::list<Unit*> targetList;
        float l_Radius = 10.0f;
        Unit* l_Caster = p_AreaTrigger->GetCaster();

        JadeCore::NearestAttackableUnitInObjectRangeCheck u_check(p_AreaTrigger, l_Caster, l_Radius);
        JadeCore::UnitListSearcher<JadeCore::NearestAttackableUnitInObjectRangeCheck> searcher(p_AreaTrigger, targetList, u_check);
        p_AreaTrigger->VisitNearbyObject(l_Radius, searcher);

        if (!targetList.empty())
        {
            for (auto itr : targetList)
            {
                // Periodic absorption for Imperial Vizier Zor'lok's Force and Verve and Sonic Rings
                if (itr->GetDistance(p_AreaTrigger) > 2.0f && itr->HasAura(122706))
                    itr->RemoveAura(122706);
                else if (itr->GetDistance(p_AreaTrigger) <= 2.0f && !itr->HasAura(122706))
                    l_Caster->AddAura(122706, itr);
            }
        }
    }

    void OnRemove(AreaTrigger* p_AreaTrigger)
    {
        std::list<Player*> playerList;
        p_AreaTrigger->GetPlayerListInGrid(playerList, 200.0f);

        for (auto player : playerList)
        if (player->HasAura(122706))
            player->RemoveAura(122706);
    }
};

class AreaTrigger_get_away : public MS::AreaTriggerEntityScript
{
public:
    AreaTrigger_get_away()
        : MS::AreaTriggerEntityScript("at_get_away")
    {
    }

    MS::AreaTriggerEntityScript* GetAI()
    {
        return new AreaTrigger_get_away();
    }

    void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time)
    {
        std::list<Player*> playerList;
        p_AreaTrigger->GetPlayerListInGrid(playerList, 60.0f);

        Position pos;
        p_AreaTrigger->GetPosition(&pos);

        Unit* l_Caster = p_AreaTrigger->GetCaster();

        for (auto player : playerList)
        {
            if (player->IsWithinDist(l_Caster, 40.0f, false))
            {
                if (player->isAlive() && !player->hasForcedMovement)
                    player->SendApplyMovementForce(true, pos, -3.0f);
                else if (!player->isAlive() && player->hasForcedMovement)
                    player->SendApplyMovementForce(false, pos);
            }
            else if (player->hasForcedMovement)
                player->SendApplyMovementForce(false, pos);
        }
    }

    void OnRemove(AreaTrigger* p_AreaTrigger)
    {
        std::list<Player*> playerList;
        p_AreaTrigger->GetPlayerListInGrid(playerList, 100.0f);

        Position pos;
        p_AreaTrigger->GetPosition(&pos);

        for (auto player : playerList)
            player->SendApplyMovementForce(false, pos);
    }
};

class AreaTrigger_draw_power : public MS::AreaTriggerEntityScript
{
public:
    AreaTrigger_draw_power()
        : MS::AreaTriggerEntityScript("at_draw_power")
    {
    }

    MS::AreaTriggerEntityScript* GetAI()
    {
        return new AreaTrigger_draw_power();
    }

    void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time)
    {
        std::list<Unit*> targetList;
        float l_Radius = 30.0f;
        Unit* l_Caster = p_AreaTrigger->GetCaster();

        JadeCore::NearestAttackableUnitInObjectRangeCheck u_check(p_AreaTrigger, l_Caster, l_Radius);
        JadeCore::UnitListSearcher<JadeCore::NearestAttackableUnitInObjectRangeCheck> searcher(p_AreaTrigger, targetList, u_check);
        p_AreaTrigger->VisitNearbyObject(l_Radius, searcher);

        for (auto itr : targetList)
        {
            if (itr->IsInAxe(l_Caster, p_AreaTrigger, 2.0f))
            {
                if (!itr->HasAura(116663))
                    l_Caster->AddAura(116663, itr);
            }
            else
                itr->RemoveAurasDueToSpell(116663);
        }
    }
};

class AreaTrigger_healing_sphere_2 : public MS::AreaTriggerEntityScript
{
public:
    AreaTrigger_healing_sphere_2()
        : MS::AreaTriggerEntityScript("at_healing_sphere_2")
    {
    }

    MS::AreaTriggerEntityScript* GetAI()
    {
        return new AreaTrigger_healing_sphere_2();
    }

    void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time)
    {
        std::list<Unit*> targetList;
        float l_Radius = 1.0f;
        Unit* l_Caster = p_AreaTrigger->GetCaster();

        JadeCore::AnyFriendlyUnitInObjectRangeCheck u_check(p_AreaTrigger, l_Caster, l_Radius);
        JadeCore::UnitListSearcher<JadeCore::AnyFriendlyUnitInObjectRangeCheck> searcher(p_AreaTrigger, targetList, u_check);
        p_AreaTrigger->VisitNearbyObject(l_Radius, searcher);

        if (!targetList.empty())
        {
            for (auto itr : targetList)
            {
                if (itr->GetGUID() == l_Caster->GetGUID())
                {
                    l_Caster->CastSpell(itr, 125355, true); // Heal for 15% of life
                    p_AreaTrigger->SetDuration(0);
                    return;
                }
            }
        }
    }
};

class AreaTrigger_gift_of_the_serpent : public MS::AreaTriggerEntityScript
{
public:
    AreaTrigger_gift_of_the_serpent()
        : MS::AreaTriggerEntityScript("at_gift_of_the_serpent")
    {
    }

    MS::AreaTriggerEntityScript* GetAI()
    {
        return new AreaTrigger_gift_of_the_serpent();
    }

    void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time)
    {
        std::list<Unit*> targetList;
        float l_Radius = 1.0f;
        Unit* l_Caster = p_AreaTrigger->GetCaster();

        JadeCore::AnyFriendlyUnitInObjectRangeCheck u_check(p_AreaTrigger, p_AreaTrigger->GetCaster(), l_Radius);
        JadeCore::UnitListSearcher<JadeCore::AnyFriendlyUnitInObjectRangeCheck> searcher(p_AreaTrigger, targetList, u_check);
        p_AreaTrigger->VisitNearbyObject(l_Radius, searcher);

        if (!targetList.empty())
        {
            for (auto itr : targetList)
            {
                l_Caster->CastSpell(itr, 124041, true); // Gift of the Serpent heal
                p_AreaTrigger->SetDuration(0);
                return;
            }
        }
    }
};

class AreaTrigger_chi_sphere_afterlife : public MS::AreaTriggerEntityScript
{
public:
    AreaTrigger_chi_sphere_afterlife()
        : MS::AreaTriggerEntityScript("at_chi_sphere_afterlife")
    {
    }

    MS::AreaTriggerEntityScript* GetAI()
    {
        return new AreaTrigger_chi_sphere_afterlife();
    }

    void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time)
    {
        std::list<Unit*> targetList;
        float l_Radius = 1.0f;
        Unit* l_Caster = p_AreaTrigger->GetCaster();

        JadeCore::AnyFriendlyUnitInObjectRangeCheck u_check(p_AreaTrigger, l_Caster, l_Radius);
        JadeCore::UnitListSearcher<JadeCore::AnyFriendlyUnitInObjectRangeCheck> searcher(p_AreaTrigger, targetList, u_check);
        p_AreaTrigger->VisitNearbyObject(l_Radius, searcher);

        if (!targetList.empty())
        {
            for (auto itr : targetList)
            {
                if (itr->GetGUID() == l_Caster->GetGUID())
                {
                    l_Caster->CastSpell(itr, 121283, true); // Restore 1 Chi
                    p_AreaTrigger->SetDuration(0);
                    return;
                }
            }
        }
    }
};

class AreaTrigger_angelic_feather : public MS::AreaTriggerEntityScript
{
public:
    AreaTrigger_angelic_feather()
        : MS::AreaTriggerEntityScript("at_angelic_feather")
    {
    }

    MS::AreaTriggerEntityScript* GetAI()
    {
        return new AreaTrigger_angelic_feather();
    }

    void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time)
    {
        std::list<Unit*> targetList;
        float l_Radius = 1.0f;
        Unit* l_Caster = p_AreaTrigger->GetCaster();

        JadeCore::AnyFriendlyUnitInObjectRangeCheck u_check(p_AreaTrigger, l_Caster, l_Radius);
        JadeCore::UnitListSearcher<JadeCore::AnyFriendlyUnitInObjectRangeCheck> searcher(p_AreaTrigger, targetList, u_check);
        p_AreaTrigger->VisitNearbyObject(l_Radius, searcher);

        if (!targetList.empty())
        {
            for (auto itr : targetList)
            {
                l_Caster->CastSpell(itr, 121557, true); // Angelic Feather increase speed
                p_AreaTrigger->SetDuration(0);
                return;
            }
        }
    }
};

class AreaTrigger_gift_of_the_ox : public MS::AreaTriggerEntityScript
{
public:
    AreaTrigger_gift_of_the_ox()
        : MS::AreaTriggerEntityScript("at_gift_of_the_ox")
    {
    }

    MS::AreaTriggerEntityScript* GetAI()
    {
        return new AreaTrigger_gift_of_the_ox();
    }

    void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time)
    {
        std::list<Unit*> targetList;
        float l_Radius = 1.0f;
        Unit* l_Caster = p_AreaTrigger->GetCaster();

        JadeCore::AnyFriendlyUnitInObjectRangeCheck u_check(p_AreaTrigger, l_Caster, l_Radius);
        JadeCore::UnitListSearcher<JadeCore::AnyFriendlyUnitInObjectRangeCheck> searcher(p_AreaTrigger, targetList, u_check);
        p_AreaTrigger->VisitNearbyObject(l_Radius, searcher);

        for (auto itr : targetList)
        {
            if (itr->GetGUID() != l_Caster->GetGUID())
                continue;

            l_Caster->CastSpell(itr, 124507, true); // Gift of the Ox - Heal
            p_AreaTrigger->SetDuration(0);
            return;
        }
    }
};

class AreaTrigger_down_draft : public MS::AreaTriggerEntityScript
{
public:
    AreaTrigger_down_draft()
        : MS::AreaTriggerEntityScript("at_down_draft")
    {
    }

    MS::AreaTriggerEntityScript* GetAI()
    {
        return new AreaTrigger_down_draft();
    }

    void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time)
    {
        std::list<Player*> playerList;
        p_AreaTrigger->GetPlayerListInGrid(playerList, 40.0f);

        Position pos;
        p_AreaTrigger->GetPosition(&pos);

        for (auto player : playerList)
        {
            if (player->IsWithinDist(p_AreaTrigger->GetCaster(), 30.0f, false))
            {
                if (player->isAlive() && !player->hasForcedMovement)
                    player->SendApplyMovementForce(true, pos, -12.0f);
                else if (!player->isAlive() && player->hasForcedMovement)
                    player->SendApplyMovementForce(false, pos);
            }
            else if (player->hasForcedMovement)
                player->SendApplyMovementForce(false, pos);
        }
    }

    void OnRemove(AreaTrigger* p_AreaTrigger, uint32(p_time))
    {
        std::list<Player*> playerList;
        p_AreaTrigger->GetPlayerListInGrid(playerList, 100.0f);

        Position pos;
        p_AreaTrigger->GetPosition(&pos);

        for (auto player : playerList)
            player->SendApplyMovementForce(false, pos);
    }
};

class AreaTrigger_zen_sphere_1 : public MS::AreaTriggerEntityScript
{
public:
    AreaTrigger_zen_sphere_1()
        : MS::AreaTriggerEntityScript("at_zen_sphere_1")
    {
    }

    MS::AreaTriggerEntityScript* GetAI()
    {
        return new AreaTrigger_zen_sphere_1();
    }

    void OnRemove(AreaTrigger* p_AreaTrigger, uint32(p_time))
    {
        if (int32(p_AreaTrigger->GetDuration()) - int32(p_time) > 0 || p_AreaTrigger->GetDuration() == 0)
            return;

        if (!p_AreaTrigger->GetCaster())
            return;

        p_AreaTrigger->GetCaster()->CastSpell(p_AreaTrigger->GetPositionX(), p_AreaTrigger->GetPositionY(), p_AreaTrigger->GetPositionZ(), 135914, true);
    }
};

class AreaTrigger_zen_sphere_2 : public MS::AreaTriggerEntityScript
{
public:
    AreaTrigger_zen_sphere_2()
        : MS::AreaTriggerEntityScript("at_zen_sphere_2")
    {
    }

    MS::AreaTriggerEntityScript* GetAI()
    {
        return new AreaTrigger_zen_sphere_2();
    }

    void OnRemove(AreaTrigger* p_AreaTrigger, uint32(p_time))
    {
        if (int32(p_AreaTrigger->GetDuration()) - int32(p_time) > 0 || p_AreaTrigger->GetDuration() == 0)
            return;

        if (!p_AreaTrigger->GetCaster())
            return;

        p_AreaTrigger->GetCaster()->CastSpell(p_AreaTrigger->GetPositionX(), p_AreaTrigger->GetPositionY(), p_AreaTrigger->GetPositionZ(), 135920, true);
    }
};

class AreaTrigger_vileblood_serum : public MS::AreaTriggerEntityScript
{
public:
    AreaTrigger_vileblood_serum()
        : MS::AreaTriggerEntityScript("at_vileblood_serum")
    {
    }

    MS::AreaTriggerEntityScript* GetAI()
    {
        return new AreaTrigger_vileblood_serum();
    }

    void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time)
    {
        std::list<Unit*> l_TargetList;
        float l_Radius = 2.0f;

        JadeCore::AnyFriendlyUnitInObjectRangeCheck l_Check(p_AreaTrigger, p_AreaTrigger->GetCaster(), l_Radius);
        JadeCore::UnitListSearcher<JadeCore::AnyFriendlyUnitInObjectRangeCheck> l_Searcher(p_AreaTrigger, l_TargetList, l_Check);
        p_AreaTrigger->VisitNearbyObject(l_Radius, l_Searcher);

        for (Unit* l_Unit : l_TargetList)
        {
            if (l_Unit->GetDistance(p_AreaTrigger) <= l_Radius)
                l_Unit->CastSpell(l_Unit, 161288, true);
        }
    }
};

class AreaTrigger_noxious_spit : public MS::AreaTriggerEntityScript
{
public:
    AreaTrigger_noxious_spit()
        : MS::AreaTriggerEntityScript("at_noxious_spit")
    {
    }

    MS::AreaTriggerEntityScript* GetAI()
    {
        return new AreaTrigger_noxious_spit();
    }

    void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time)
    {
        std::list<Unit*> l_TargetList;
        float l_Radius = 2.0f;

        JadeCore::NearestAttackableUnitInObjectRangeCheck l_Check(p_AreaTrigger, p_AreaTrigger->GetCaster(), l_Radius);
        JadeCore::UnitListSearcher<JadeCore::NearestAttackableUnitInObjectRangeCheck> l_Searcher(p_AreaTrigger, l_TargetList, l_Check);
        p_AreaTrigger->VisitNearbyObject(l_Radius, l_Searcher);

        for (Unit* l_Unit : l_TargetList)
        {
            if (l_Unit->GetExactDist2d(p_AreaTrigger) > l_Radius)
                continue;

            p_AreaTrigger->GetCaster()->CastSpell(l_Unit, 136962, true);
            p_AreaTrigger->SetDuration(0);
            return;
        }
    }
};

void AddSC_areatrigger_scripts()
{
    new AreaTrigger_zen_sphere_2();
    new AreaTrigger_zen_sphere_1();
    new AreaTrigger_noxious_spit();
    new AreaTrigger_vileblood_serum();
    new AreaTrigger_down_draft();
    new AreaTrigger_gift_of_the_ox();
    new AreaTrigger_angelic_feather();
    new AreaTrigger_chi_sphere_afterlife();
    new AreaTrigger_gift_of_the_serpent();
    new AreaTrigger_healing_sphere_2();
    new AreaTrigger_draw_power();
    new AreaTrigger_get_away();
    new AreaTrigger_cancelling_noise_area_trigger();
    new AreaTrigger_amethyst_pool();
    new AreaTrigger_rune_of_power();
    new AreaTrigger_cancel_barrier();
    new AreaTrigger_healing_sphere();
    new AreaTrigger_ursol_vortex();
    new AreaTrigger_power_word_barrier();
    new AreaTrigger_ice_trap();

    new AreaTrigger_at_coilfang_waterfall();
    new AreaTrigger_at_legion_teleporter();
    new AreaTrigger_at_stormwright_shelf();
    new AreaTrigger_at_scent_larkorwi();
    new AreaTrigger_at_last_rites();
    new AreaTrigger_at_sholazar_waygate();
    new AreaTrigger_at_nats_landing();
    new AreaTrigger_at_bring_your_orphan_to();
    new AreaTrigger_at_brewfest();
    new AreaTrigger_at_area_52_entrance();
    new AreaTrigger_at_bael_modan();
    new AreaTrigger_at_klaxxi_vess();
    new AreaTrigger_at_farmer_fung();
    new areatrigger_at_serpent_nests();
    new AreaTrigger_at_mason_s_folly();
}
