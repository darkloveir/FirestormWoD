////////////////////////////////////////////////////////////////////////////////
///
///  MILLENIUM-STUDIO
///  Copyright 2015 Millenium-studio SARL
///  All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

# include "the_everbloom.hpp"

enum eXeritacSpells
{
    /// Descend
    SpellDecsendDummy        = 172643,
    SpellDecsendBeam         = 169322,
    SpellDescendEffectDamage = 169275, 
    SpellDescendDamage       = 169278, 
    SpellWebCrawl            = 169293,

    SpellGasVolleyDummy      = 169382,
    SpellGasVolleyMissile    = 169383,
    SpellToxicGasDamage      = 169223, 
    SpellToxicGasAreaTrigger = 169224,
    SpellToxicBolt           = 169375,
    SpellToxicGas            = 169218,
    SpellVenoumousSting      = 169376, 
    SpellSwipeDamage         = 169371,
    SpellConsume             = 169248,
    SpellConsumeAura         = 169249,
    SpellVenomSpray          = 172727,
    SpellToxicBlood          = 169218,
    SpellToxicitiy           = 169219,
    SpellToxicBloodExplode   = 169267,
    SpellFixate              = 173080,
    SpellBurst               = 173081,
    SpellInhaleDummy         = 169233
};

enum eXeritacEvents
{
    EventConsume = 1,
    EventDescend,
    EventDecsendDummy,
    EventGasVolley,
    EventToxicGas,
    EventToxicBolt,
    EventToxicAura,
    EventVenomousString,
    EventWebCrawl,
    EventToxicSpiderling,
    EventVenomCrazedPaleOne,
    EventGorgendBusters,
    EventVenomSprayers,
    EventExplode,
    EventSwipe,
    EventCancelSwipe,
    EventInhale,
    EventRemoveBeam,
    EventMoveHack,
    EventInitialBeam,
    EventMoveSecondPhaseHacked
};

enum eXeritacCreatures
{
    TriggerGas         = 213152,
    TriggerWebCrawl    = 534536,
    TriggerBeamTrigger = 432521
};

enum eXeritacActions
{
    ActionCountSpiderLing = 1,
    ActionInhale,
    ActionCount,
    ActionConsumeEffect
};

Position g_PositionRandomMovements[5] =
{
    { 931.20f, 1441.587f, 85.600f, 3.289805f },
    { 908.84f, 1449.853f, 85.600f, 0.615524f },
    { 921.09f, 1456.374f, 85.600f, 5.465359f },
    { 938.89f, 1443.991f, 85.600f, 3.359706f },
    { 930.02f, 1434.803f, 85.600f, 2.946586f }
};

Position g_PositionitionPaleOne[2] =
{
    { 926.44f, 1413.111f, 66.364f, 2.336323f },
    { 956.79f, 1448.274f, 64.274f, 3.184339f }
};

#define HostileFaction  16
#define FriendlyFaction 35

static void ShootBeam(Unit* p_Caster)
{
    Creature* l_Beam = p_Caster->SummonCreature(eXeritacCreatures::TriggerBeamTrigger, p_Caster->GetPositionX(), p_Caster->GetPositionY(), 87.611f, p_Caster->GetOrientation(), TempSummonType::TEMPSUMMON_TIMED_DESPAWN, 8 * TimeConstants::IN_MILLISECONDS);

    if (l_Beam)
    {
        l_Beam->NearTeleportTo(p_Caster->GetPositionX(), p_Caster->GetPositionY(), 87.611f, p_Caster->GetOrientation());
        p_Caster->CastSpell(l_Beam, eXeritacSpells::SpellDecsendBeam);
        p_Caster->GetMotionMaster()->MovePoint(10, p_Caster->GetPositionX(), p_Caster->GetPositionY(), 87.611f);
    }
}

class basic_event_fix_movement_chase : public BasicEvent
{
    public:
        explicit basic_event_fix_movement_chase(Unit* p_Unit) : m_Obj(p_Unit) { }

        bool Execute(uint64 /*p_CurrTime*/, uint32 /*p_Diff*/)
        {
            if (m_Obj->GetTypeId() == TypeID::TYPEID_PLAYER)
                return false;

            if (m_Obj && m_Obj->IsAIEnabled)
            {
                if (Unit* l_Target = m_Obj->GetAI()->SelectTarget(SelectAggroTarget::SELECT_TARGET_TOPAGGRO))
                {
                    m_Obj->UpdatePosition(*m_Obj, true);

                    m_Obj->GetAI()->AttackStart(l_Target);
                    m_Obj->GetMotionMaster()->MoveChase(l_Target);
                }
            }
            return true;
        }

    private:
        Unit* m_Obj;
};

static void DescendBeam(Creature* p_Creature)
{
    if (p_Creature)
    {
        if (Creature* l_Beam = p_Creature->SummonCreature(eXeritacCreatures::TriggerBeamTrigger, *p_Creature, TEMPSUMMON_TIMED_DESPAWN, 8 * TimeConstants::IN_MILLISECONDS))
            p_Creature->CastSpell(l_Beam, eXeritacSpells::SpellDecsendBeam);

        p_Creature->GetMotionMaster()->MoveTakeoff(0, p_Creature->GetPositionX(), p_Creature->GetPositionY(), 64.589f);
        p_Creature->m_Events.AddEvent(new basic_event_fix_movement_chase(p_Creature), p_Creature->m_Events.CalculateTime(2500));
    }
}

static void DespawnCreaturesInArea(uint32 p_Entry, WorldObject* p_Object)
{
    std::list<Creature*> l_CreatureList;
    GetCreatureListWithEntryInGrid(l_CreatureList, p_Object, p_Entry, 300.0f);

    for (Creature* l_Iter : l_CreatureList)
        l_Iter->DespawnOrUnsummon();
}

class the_everbloom_remove_non_players
{
    public:
        the_everbloom_remove_non_players() { }

        bool operator()(WorldObject* p_Object)
        {
            if (p_Object->GetTypeId() == TypeID::TYPEID_PLAYER)
                return false;
            else
                return true;
        }
};

/// Xeri'tac - 84550
class boss_xeritac : public CreatureScript
{
    public:
        boss_xeritac() : CreatureScript("boss_xeritac") { }

        struct boss_xeritacAI : public BossAI
        {
            boss_xeritacAI(Creature* p_Creature) : BossAI(p_Creature, eEverbloomDatas::DataXeritac)
            {
                m_Instance = me->GetInstanceScript();
                m_Intro = false;
            }

            InstanceScript* m_Instance;
            uint32 m_TimeBetween;
            uint8  m_Count;
            uint32 m_Phase;
            uint64 m_ConsumedGUID;
            bool m_Intro;
            bool m_Descend;
            bool m_Consuming;

            void Reset() override
            {
                WebDoor();

                events.Reset();
                summons.DespawnAll();

                m_Consuming = false;
                m_Descend = false;
                m_Count = 0;
                m_TimeBetween = 6 * TimeConstants::IN_MILLISECONDS;
                m_Phase = 1;
                m_ConsumedGUID = 0;
         
                me->setFaction(HostileFaction);
                me->SetReactState(ReactStates::REACT_PASSIVE);
                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
            }

            void WebDoor()
            {
                if (m_Instance == nullptr)
                    return;

                /// Get Barrier Down
                if (GameObject* l_webDoor = m_Instance->instance->GetGameObject(m_Instance->GetData64(eEverbloomGameObjects::ObjectWebDoor)))
                {
                    /// Activate
                    l_webDoor->SetLootState(LootState::GO_READY);
                    l_webDoor->UseDoorOrButton();
                }
            }

            void MoveInLineOfSight(Unit* p_Who) override
            {
                if (p_Who->IsInWorld() && p_Who->GetTypeId() == TypeID::TYPEID_PLAYER && me->IsWithinDistInMap(p_Who, 18.0f) && !m_Intro)
                    m_Intro = true;

                if (p_Who->IsInWorld() && p_Who->GetEntry() == eEverbloomCreatures::CreatureVenomCrazedPaleOne && p_Who->IsWithinDistInMap(me, 3.0f, true) && !m_Consuming && p_Who->isAlive())
                {
                    if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                        return;

                    m_Consuming = true;
                }
            }

            void JustReachedHome() override
            {
                _JustReachedHome();

                me->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, 1);
                me->GetMotionMaster()->MovePoint(0, 935.628f, 1430.930f, 64.988f);
                me->SetAIAnimKitId(1);

                WebDoor();

                std::list<AreaTrigger*> l_GasAreatriggerList;
                me->GetAreaTriggerList(l_GasAreatriggerList, eXeritacSpells::SpellToxicGasAreaTrigger);

                for (AreaTrigger* l_Iter : l_GasAreatriggerList)
                    l_Iter->Remove(1 * TimeConstants::IN_MILLISECONDS);
            }

            void EnterCombat(Unit* p_Who) override
            {
                _EnterCombat();

                events.ScheduleEvent(eXeritacEvents::EventMoveHack, 10 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eXeritacEvents::EventDescend,  urand(20 * TimeConstants::IN_MILLISECONDS, 21 * TimeConstants::IN_MILLISECONDS));
                events.ScheduleEvent(eXeritacEvents::EventToxicSpiderling, 10 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eXeritacEvents::EventVenomSprayers, 10 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eXeritacEvents::EventVenomCrazedPaleOne, 20 * TimeConstants::IN_MILLISECONDS);

                if (m_Instance != nullptr)
                {
                    if (IsHeroic())
                        events.ScheduleEvent(eXeritacEvents::EventGorgendBusters, 10 * TimeConstants::IN_MILLISECONDS);

                    m_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_ENGAGE, me);

                    DoZoneInCombat();
                }

                m_Phase = 1;
           
                me->SetAIAnimKitId(1);
                me->SetCanFly(true);
                me->SetDisableGravity(true);
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE);

                me->GetMotionMaster()->MovePoint(10, me->GetPositionX(), me->GetPositionY(), 87.611f);
            }

            void JustDied(Unit* p_Killer) override
            {
                _JustDied();

                summons.DespawnAll();

                if (m_Instance != nullptr)
                    m_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE, me);

                std::list<AreaTrigger*> l_GasAreatriggerList;
                me->GetAreaTriggerList(l_GasAreatriggerList, eXeritacSpells::SpellToxicGasAreaTrigger);

                for (AreaTrigger* l_Iter : l_GasAreatriggerList)
                    l_Iter->Remove(1 * TimeConstants::IN_MILLISECONDS);

                WebDoor();
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eEverbloomActions::ActionCounting:
                        m_Count++;

                        if (m_Count >= 5 && m_Phase == 1)
                        {
                            m_Phase = 2;
                            events.Reset();

                            m_Descend = true;

                            Position l_CurrentPos;
                            me->GetPosition(&l_CurrentPos);
                            l_CurrentPos.m_positionZ = 65.619f;

                            me->GetMotionMaster()->Clear(true);
                            me->GetMotionMaster()->MovePoint(0, l_CurrentPos);
                    
                            me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                            me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);

                            events.ScheduleEvent(eXeritacEvents::EventToxicBolt, urand(7 * TimeConstants::IN_MILLISECONDS, 10 * TimeConstants::IN_MILLISECONDS));
                            events.ScheduleEvent(eXeritacEvents::EventVenomousString, 16 * TimeConstants::IN_MILLISECONDS);
                            events.ScheduleEvent(eXeritacEvents::EventGasVolley, 30 * TimeConstants::IN_MILLISECONDS);
                            events.ScheduleEvent(eXeritacEvents::EventVenomCrazedPaleOne, 20 * TimeConstants::IN_MILLISECONDS);
                            events.ScheduleEvent(eXeritacEvents::EventMoveSecondPhaseHacked, 5 * TimeConstants::IN_MILLISECONDS);

                            me->SetAIAnimKitId(1);
                            me->SetMovementAnimKitId(1);
                            me->SetMeleeAnimKitId(1);

                            if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_TOPAGGRO))
                            {
                                me->UpdatePosition(*me, true);

                                me->Attack(l_Target, true);
                                me->GetMotionMaster()->MoveChase(l_Target);
                            }

                            events.ScheduleEvent(eXeritacEvents::EventMoveSecondPhaseHacked, 6 * TimeConstants::IN_MILLISECONDS);

                            me->MonsterTextEmote("Xeri'tac descends from her web!", me->GetGUID(), true);
                        }
                        break;
                    case eXeritacActions::ActionConsumeEffect:
                        if (Creature* l_ConsumedPaleOne = sObjectAccessor->GetCreature(*me, m_ConsumedGUID))
                        {
                            l_ConsumedPaleOne->Kill(l_ConsumedPaleOne);
                            l_ConsumedPaleOne->DespawnOrUnsummon();

                            m_Consuming = false;
                        }
                        break;
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                /// Consume
                if (m_ConsumedGUID)
                {
                    if (Creature* l_ConsumedPaleOne = sObjectAccessor->GetCreature(*me, m_ConsumedGUID))
                    {
                        if (l_ConsumedPaleOne->isDead() && l_ConsumedPaleOne->IsInWorld())
                        {
                            me->CastStop();
                            m_Consuming = false;
                        }
                    }
                }

                switch (events.ExecuteEvent())
                {
                    case eXeritacEvents::EventMoveSecondPhaseHacked:
                        if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                            return;

                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_TOPAGGRO))
                            me->GetMotionMaster()->MoveChase(l_Target);

                        events.ScheduleEvent(eXeritacEvents::EventMoveSecondPhaseHacked, 5 * TimeConstants::IN_MILLISECONDS);
                        break;
                    case eXeritacEvents::EventMoveHack:
                        Position l_Position;
                        me->GetRandomNearPosition(l_Position, 10.0f);

                        l_Position.m_positionZ = 87.611f;
                        me->GetMotionMaster()->MovePoint(0, l_Position);
                        events.ScheduleEvent(eXeritacEvents::EventMoveHack, 8 * TimeConstants::IN_MILLISECONDS);
                        break;
                    case eXeritacEvents::EventInitialBeam:
                        me->CastSpell(933.281f, 1424.252f, 90.216f, eXeritacSpells::SpellDecsendBeam, true);
                        break;
                    /// Descend Phase
                    case eXeritacEvents::EventDescend:
                    {
                        events.CancelEvent(eXeritacEvents::EventMoveHack);
                        me->StopMoving();

                        DescendBeam(me);

                        events.ScheduleEvent(eXeritacEvents::EventMoveHack, 10 * TimeConstants::IN_MILLISECONDS);
                        events.ScheduleEvent(eXeritacEvents::EventDescend, 20 * TimeConstants::IN_MILLISECONDS);
                        events.ScheduleEvent(eXeritacEvents::EventDecsendDummy, 3 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eXeritacEvents::EventDecsendDummy:
                    {
                        me->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, 1);
                        me->CastSpell(me, eXeritacSpells::SpellDecsendDummy);
                        m_Descend = false;
                        m_TimeBetween = 7 * TimeConstants::IN_MILLISECONDS;
                        break;
                    }
                    /// 2nd Phase
                    case eXeritacEvents::EventToxicBolt:
                    {
                        if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                            return;

                        if (Unit* l_Victim = me->getVictim())
                            me->CastSpell(l_Victim, eXeritacSpells::SpellToxicBolt);

                        events.ScheduleEvent(eXeritacEvents::EventToxicBolt, urand(6 * TimeConstants::IN_MILLISECONDS, 8 * TimeConstants::IN_MILLISECONDS));
                        break;
                    }
                    case eXeritacEvents::EventVenomousString:
                    {
                        if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                            return;

                        if (Unit* l_Victim = me->getVictim())
                            me->CastSpell(l_Victim, eXeritacSpells::SpellVenoumousSting);

                        events.ScheduleEvent(eXeritacEvents::EventVenomousString, 15 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eXeritacEvents::EventGasVolley:
                    {
                        me->CastSpell(me, eXeritacSpells::SpellGasVolleyDummy);
                        events.ScheduleEvent(eXeritacEvents::EventGasVolley, 20 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    /// Adds
                    case eXeritacEvents::EventGorgendBusters:
                    {
                        std::list<Position> l_Position;

                        for (uint8 l_I = 0; l_I < 5; l_I++)
                            l_Position.push_back(g_PositionRandomMovements[l_I]);

                        std::list<Position>::const_iterator l_Iter = l_Position.begin();
                        std::advance(l_Iter, urand(0, l_Position.size() - 1));

                        if (Creature* l_Spiderling = me->SummonCreature(eEverbloomCreatures::CreatureGorgendBusters, *l_Iter, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                            DescendBeam(l_Spiderling);

                        events.ScheduleEvent(eXeritacEvents::EventGorgendBusters, 30 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eXeritacEvents::EventToxicSpiderling:
                    {
                        std::list<Position> l_Position;

                        for (uint8 l_I = 0; l_I < 5; l_I++)
                            l_Position.push_back(g_PositionRandomMovements[l_I]);

                        std::list<Position>::const_iterator l_Iter = l_Position.begin();
                        std::advance(l_Iter, urand(0, l_Position.size() - 1));

                        if (Creature* l_Spiderling = me->SummonCreature(eEverbloomCreatures::CreatureToxicSpiderling, *l_Iter, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                            DescendBeam(l_Spiderling);

                        me->MonsterTextEmote("|TInterface\\Icons\\achievement_halloween_rottenegg_01.blp:20|tXeri'tac begins unleashing Toxic Spiderlings down on you!", me->GetGUID());

                        events.ScheduleEvent(eXeritacEvents::EventToxicSpiderling, 30 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eXeritacEvents::EventVenomSprayers:
                    {
                        std::list<Position> l_Position;

                        for (uint8 l_I = 0; l_I < 5; l_I++)
                            l_Position.push_back(g_PositionRandomMovements[l_I]);

                        std::list<Position>::const_iterator l_Iter = l_Position.begin();
                        std::advance(l_Iter, urand(0, l_Position.size() - 1));
           
                        if (Creature* l_Spiderling = me->SummonCreature(eEverbloomCreatures::CreatureVenomSprayer, *l_Iter, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN))
                            DescendBeam(l_Spiderling);

                        events.ScheduleEvent(eXeritacEvents::EventVenomSprayers, 30 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eXeritacEvents::EventVenomCrazedPaleOne:
                    {
                        me->SummonCreature(eEverbloomCreatures::CreatureVenomCrazedPaleOne, g_PositionitionPaleOne[urand(0, 1)], TempSummonType::TEMPSUMMON_MANUAL_DESPAWN);
                        events.ScheduleEvent(eXeritacEvents::EventVenomCrazedPaleOne, 35 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_xeritacAI(p_Creature);
        }
};

/// Vendom Sprayer - 86547
class the_everbloom_venom_sprayer : public CreatureScript
{
    public:
        the_everbloom_venom_sprayer() : CreatureScript("the_everbloom_venom_sprayer") { }

        struct the_everbloom_creaturesAI : public ScriptedAI
        {
            the_everbloom_creaturesAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                events.Reset();

                DoZoneInCombat();
                me->SetDisableGravity(true);
                me->SetCanFly(true);
                me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                events.ScheduleEvent(eXeritacEvents::EventRemoveBeam, 3 * TimeConstants::IN_MILLISECONDS);
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                events.ScheduleEvent(eXeritacEvents::EventMoveHack, 3 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eXeritacEvents::EventVenomSprayers, urand(7 * TimeConstants::IN_MILLISECONDS, 9 * TimeConstants::IN_MILLISECONDS));
            }

            void UpdateAI(const uint32 p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eXeritacEvents::EventMoveHack:
                        if (Unit* l_Target = me->getVictim())
                            me->GetMotionMaster()->MoveFollow(l_Target, 0, 0, MOTION_SLOT_ACTIVE);

                        events.ScheduleEvent(eXeritacEvents::EventMoveHack, 3000);
                        break;
                    case eXeritacEvents::EventVenomSprayers:
                        me->CastSpell(me, eXeritacSpells::SpellVenomSpray);
                        events.ScheduleEvent(eXeritacEvents::EventToxicGas, urand(7 * TimeConstants::IN_MILLISECONDS, 9 * TimeConstants::IN_MILLISECONDS));
                        break;
                    case eXeritacEvents::EventRemoveBeam:
                        me->RemoveAura(eXeritacSpells::SpellDecsendBeam);
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new the_everbloom_creaturesAI(p_Creature);
        }
};

/// Crazed Pale One - 84554
class the_everbloom_venom_crazed_pale_one : public CreatureScript
{
    public:
        the_everbloom_venom_crazed_pale_one() : CreatureScript("the_everbloom_venom_crazed_pale_one") { }

        struct the_everbloom_creaturesAI : public ScriptedAI
        {
            the_everbloom_creaturesAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            bool m_Transformed;

            void Reset() override
            {
                events.Reset();

                m_Transformed = false;
                DoZoneInCombat();
                me->setFaction(HostileFaction);

                me->SetSpeed(UnitMoveType::MOVE_RUN, 0.6f, true);
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                events.ScheduleEvent(eXeritacEvents::EventSwipe, 2 * TimeConstants::IN_MILLISECONDS);
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eXeritacActions::ActionInhale:
                        me->CastSpell(me, eXeritacSpells::SpellInhaleDummy);
                        break;
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                if (me->FindNearestCreature(eXeritacCreatures::TriggerGas, 1.5f) && !m_Transformed)
                {
                    m_Transformed = true;

                    me->CastSpell(me, eXeritacSpells::SpellToxicGas);
                }
    
                switch (events.ExecuteEvent())
                {
                    case eXeritacEvents::EventSwipe:
                        me->CastSpell(me, eXeritacSpells::SpellSwipeDamage); ///< Swipe

                        events.ScheduleEvent(eXeritacEvents::EventSwipe, 3 * TimeConstants::IN_MILLISECONDS);
                        events.ScheduleEvent(eXeritacEvents::EventCancelSwipe, 15 * TimeConstants::IN_MILLISECONDS);
                        break;
                    case eXeritacEvents::EventCancelSwipe:
                        events.CancelEvent(eXeritacEvents::EventSwipe);
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new the_everbloom_creaturesAI(p_Creature);
        }
};

/// Toxic Spiderling - 84552
class the_everbloom_toxic_spiderling : public CreatureScript
{
    public:
        the_everbloom_toxic_spiderling() : CreatureScript("the_everbloom_toxic_spiderling") { }

        struct the_everbloom_creaturesAI : public ScriptedAI
        {
            the_everbloom_creaturesAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = me->GetInstanceScript();
            }

            InstanceScript* m_Instance;
            bool m_Explode;

            void Reset() override
            {
                events.Reset();

                DoZoneInCombat();
                me->RemoveAllAuras();
                me->SetDisableGravity(true);
                me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                m_Explode = false;

                events.ScheduleEvent(eXeritacEvents::EventRemoveBeam, 3 * TimeConstants::IN_MILLISECONDS);
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                me->CastSpell(me, eXeritacSpells::SpellToxicBlood);
            
                events.ScheduleEvent(eXeritacEvents::EventMoveHack, 3 * TimeConstants::IN_MILLISECONDS);
                events.ScheduleEvent(eXeritacEvents::EventToxicBolt, 8 * TimeConstants::IN_MILLISECONDS);
            }

            void JustDied(Unit* p_Killer) override
            {
                if (m_Instance != nullptr)
                {
                    if (Creature* Xeritac = m_Instance->instance->GetCreature(m_Instance->GetData64(eEverbloomBosses::BossXeritac)))
                    {
                        if (Xeritac->IsAIEnabled)
                            Xeritac->AI()->DoAction(eEverbloomActions::ActionCounting);

                        me->SummonCreature(eXeritacCreatures::TriggerGas, *me, TEMPSUMMON_MANUAL_DESPAWN);
                    }
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (me->HasAura(eEverbloomSpells::SpellToxicity) && me->GetAura(eEverbloomSpells::SpellToxicity)->GetStackAmount() >= 10 && !m_Explode)
                {
                    m_Explode = true;
                    me->RemoveAllAuras();

                    me->CastSpell(me, eXeritacSpells::SpellToxicBloodExplode);
                }

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eXeritacEvents::EventMoveHack:
                        if (Unit* l_Target = me->getVictim())
                            me->GetMotionMaster()->MoveFollow(l_Target, 0, 0, MOTION_SLOT_ACTIVE);

                        events.ScheduleEvent(eXeritacEvents::EventMoveHack, 3000);
                        break;
                    case eXeritacEvents::EventToxicBolt:
                        if (Unit* l_Target = me->getVictim())
                            me->CastSpell(l_Target, eXeritacSpells::SpellToxicBolt);

                        events.ScheduleEvent(eXeritacEvents::EventToxicBolt, 8 * TimeConstants::IN_MILLISECONDS);
                        break;
                    case eXeritacEvents::EventRemoveBeam:
                        me->RemoveAura(eXeritacSpells::SpellDecsendBeam);
                        break;
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new the_everbloom_creaturesAI(p_Creature);
        }
};

/// Gorged Bursters - 86552
class the_everbloom_gorged_bruster : public CreatureScript
{
    public:
        the_everbloom_gorged_bruster() : CreatureScript("the_everbloom_gorged_bruster") { }

        struct the_everbloom_creaturesAI : public ScriptedAI
        {
            the_everbloom_creaturesAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            uint64 m_Target = 0;
            bool m_Transformed;
            bool m_Explosion;

            void Reset() override
            {
                events.Reset();

                m_Transformed = false;
                m_Explosion = false;
                DoZoneInCombat();
                me->SetDisableGravity(true);
                me->SetSpeed(UnitMoveType::MOVE_RUN, 0.3f, true);

                if (Unit* l_Random = SelectTarget(SelectAggroTarget::SELECT_TARGET_RANDOM, 0, 100.0f, true))
                {
                    me->GetMotionMaster()->MoveFollow(l_Random, 0, 0, MOTION_SLOT_ACTIVE);
                    me->CastSpell(l_Random, eXeritacSpells::SpellFixate);

                    m_Target = l_Random->GetGUID();
                }

                events.ScheduleEvent(eXeritacEvents::EventMoveHack, 3000);
                events.ScheduleEvent(eXeritacEvents::EventRemoveBeam, 3 * TimeConstants::IN_MILLISECONDS);
            }

            void OnAddThreat(Unit* p_Victim, float& p_Threat, SpellSchoolMask /*p_SchoolMask*/, SpellInfo const* /*p_ThreatSpell*/) override
            {
                if (m_Target != 0)
                {
                    if (p_Victim->GetGUID() != m_Target)
                        p_Threat = 0;

                    return;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(p_Diff);

                if (!m_Explosion)
                {
                    /// Burst hardcoded
                    if (Player* l_Player = sObjectAccessor->GetPlayer(*me, m_Target))
                    {
                        me->SetSpeed(UnitMoveType::MOVE_RUN, 0.3f, true);

                        if (!me->isMoving())
                            me->GetMotionMaster()->MoveFollow(l_Player, 0, 0, MovementSlot::MOTION_SLOT_ACTIVE);

                        if (l_Player->IsWithinDistInMap(me, 1.0f))
                        {
                            m_Explosion = true;

                            me->CastSpell(l_Player, eXeritacSpells::SpellBurst);
                        }
                    }
                }

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (events.ExecuteEvent())
                {
                    case eXeritacEvents::EventRemoveBeam:
                        me->RemoveAura(eXeritacSpells::SpellDecsendBeam);
                        break;
                    case eXeritacEvents::EventMoveHack:
                        if (Unit * l_Target = me->getVictim())
                            me->Attack(l_Target, true);

                        events.ScheduleEvent(eXeritacEvents::EventMoveHack, 3 * TimeConstants::IN_MILLISECONDS);
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new the_everbloom_creaturesAI(p_Creature);
        }
};

/// Xeritac Beam - 432521
class the_everbloom_xeritac_beam : public CreatureScript
{
    public:
        the_everbloom_xeritac_beam() : CreatureScript("the_everbloom_xeritac_beam") { }

        struct the_everbloom_creaturesAI : public ScriptedAI
        {
            the_everbloom_creaturesAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                me->SetReactState(ReactStates::REACT_PASSIVE);

                me->SetDisableGravity(true);
                me->SetCanFly(true);

                me->setFaction(35);
                me->NearTeleportTo(me->GetPositionX(), me->GetPositionY(), 87.808f, me->GetOrientation());
            }

            void OnAddThreat(Unit* p_Victim, float& p_Threat, SpellSchoolMask /*p_SchoolMask*/, SpellInfo const* /*p_ThreatSpell*/) override
            {
                p_Threat = 0;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new the_everbloom_creaturesAI(p_Creature);
        }
};

/// Trigger Gas - 213152
class the_everbloom_trigger_gas : public CreatureScript
{
    public:
        the_everbloom_trigger_gas() : CreatureScript("the_everbloom_trigger_gas") { }

        struct the_everbloom_creaturesAI : public ScriptedAI
        {
            the_everbloom_creaturesAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            void Reset() override
            {
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_NOT_SELECTABLE);
                me->SetReactState(ReactStates::REACT_PASSIVE);
                me->SetDisableGravity(true);
                me->SetCanFly(true);
                me->setFaction(35);
                me->CastSpell(me, eXeritacSpells::SpellToxicGasAreaTrigger);
            }

            void OnAddThreat(Unit* p_Victim, float& p_Threat, SpellSchoolMask /*p_SchoolMask*/, SpellInfo const* /*p_ThreatSpell*/) override
            {
                p_Threat = 0;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new the_everbloom_creaturesAI(p_Creature);
        }
};

/// Descend - 172643
class the_everbloom_descend : public SpellScriptLoader
{
    public:
        the_everbloom_descend() : SpellScriptLoader("the_everbloom_descend") { }

        class the_everbloom_spells : public SpellScript
        {
            PrepareSpellScript(the_everbloom_spells);

            bool Load()
            {
                SpellInfo* l_Spell = const_cast<SpellInfo*>(GetSpellInfo());
                l_Spell->Effects[2].TriggerSpell = 0;
                return true;
            }

            void CorrectTargets(std::list<WorldObject*>& p_Targets)
            {
                if (!GetCaster() || p_Targets.empty())
                    return;

                p_Targets.remove_if(the_everbloom_remove_non_players());
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(the_everbloom_spells::CorrectTargets, SpellEffIndex::EFFECT_1, Targets::TARGET_DEST_DEST);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(the_everbloom_spells::CorrectTargets, SpellEffIndex::EFFECT_1, Targets::TARGET_UNIT_DEST_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new the_everbloom_spells();
        }
};

/// Descend - 169278 
class the_everbloom_descend_trigger : public SpellScriptLoader
{
    public:
        the_everbloom_descend_trigger() : SpellScriptLoader("the_everbloom_descend_trigger") { }

        class the_everbloom_spells : public SpellScript
        {
            PrepareSpellScript(the_everbloom_spells);

            void CorrectTargets(std::list<WorldObject*>& p_Targets)
            {
                if (!GetCaster() || p_Targets.empty())
                    return;

                p_Targets.remove_if(the_everbloom_remove_non_players());
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(the_everbloom_spells::CorrectTargets, SpellEffIndex::EFFECT_0, Targets::TARGET_SRC_CASTER);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(the_everbloom_spells::CorrectTargets, SpellEffIndex::EFFECT_0, Targets::TARGET_UNIT_SRC_AREA_ENTRY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(the_everbloom_spells::CorrectTargets, SpellEffIndex::EFFECT_1, Targets::TARGET_SRC_CASTER);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(the_everbloom_spells::CorrectTargets, SpellEffIndex::EFFECT_1, Targets::TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new the_everbloom_spells();
        }
};

/// Gaseous Volley - 169382 
class the_everbloom_gaseous_volley : public SpellScriptLoader
{
    public:
        the_everbloom_gaseous_volley() : SpellScriptLoader("the_everbloom_gaseous_volley") { }

        class everbloom_spells : public SpellScript
        {
            PrepareSpellScript(everbloom_spells);

            void HandleDummy(SpellEffIndex /*p_EffIndex*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    std::list<Player*> l_GaseousVolleyList;
                    l_Caster->GetPlayerListInGrid(l_GaseousVolleyList, 200.0f);

                    if (l_GaseousVolleyList.empty())
                        return;

                    std::list<Player*>::const_iterator l_It = l_GaseousVolleyList.begin();
                    std::advance(l_It, urand(0, (l_GaseousVolleyList.size() - 2) ? l_GaseousVolleyList.size() > 1 : 1));

                    l_Caster->CastSpell((*l_It), eXeritacSpells::SpellGasVolleyMissile, true);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(everbloom_spells::HandleDummy, SpellEffIndex::EFFECT_0, SpellEffects::SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new everbloom_spells();
        }
};

/// Consume - 169246  
class the_everbloom_consume : public SpellScriptLoader
{
    public:
        the_everbloom_consume() : SpellScriptLoader("the_everbloom_consume") { }

        class everbloom_spells : public SpellScript
        {
            PrepareSpellScript(everbloom_spells);

            void HandleAfterCast()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (l_Caster->GetAI())
                        l_Caster->GetAI()->DoAction(eXeritacActions::ActionConsumeEffect);
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(everbloom_spells::HandleAfterCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new everbloom_spells();
        }
};

/// Burst- 173081  
class the_everbloom_burst : public SpellScriptLoader
{
    public:
        the_everbloom_burst() : SpellScriptLoader("the_everbloom_burst") { }

        class everbloom_spells : public SpellScript
        {
            PrepareSpellScript(everbloom_spells);

            void HandleDummy(SpellEffIndex /*p_EffIndex*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (l_Caster->GetTypeId() == TypeID::TYPEID_PLAYER)
                        return;

                    l_Caster->Kill(l_Caster);
                    l_Caster->ToCreature()->DespawnOrUnsummon(2 * TimeConstants::IN_MILLISECONDS);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(everbloom_spells::HandleDummy, SpellEffIndex::EFFECT_1, SpellEffects::SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new everbloom_spells();
        }
};

/// Toxic Blood Explosion - 169267  
class the_everbloom_toxic_blood_explosion : public SpellScriptLoader
{
    public:
        the_everbloom_toxic_blood_explosion() : SpellScriptLoader("the_everbloom_toxic_blood_explosion") { }

        class everbloom_spells : public SpellScript
        {
            PrepareSpellScript(everbloom_spells);

            void HandleDummy(SpellEffIndex /*p_EffIndex*/)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    l_Caster->CastSpell(l_Caster, eXeritacSpells::SpellToxicGasAreaTrigger);
                    l_Caster->Kill(l_Caster);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(everbloom_spells::HandleDummy, SpellEffIndex::EFFECT_0, SpellEffects::SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new everbloom_spells();
        }
};

/// Descend - 169278  
class the_everbloom_descend_damage_trigger : public SpellScriptLoader
{
    public:
        the_everbloom_descend_damage_trigger() : SpellScriptLoader("the_everbloom_descend_damage_trigger") { }

        class everbloom_spells : public SpellScript
        {
            PrepareSpellScript(everbloom_spells);

            void HandlCastSpell()
            {
                if (Unit* l_Caster = GetCaster())
                {
                    Creature* l_Beam = GetCaster()->SummonCreature(eXeritacCreatures::TriggerBeamTrigger, GetCaster()->GetPositionX(), GetCaster()->GetPositionY(), 87.611f, GetCaster()->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 8 * TimeConstants::IN_MILLISECONDS);

                    if (l_Beam)
                        GetCaster()->GetMotionMaster()->MoveCharge(GetCaster()->GetPositionX(), GetCaster()->GetPositionY(), 87.611f);
                }
            }

            void Register() override
            {
                OnCast += SpellCastFn(everbloom_spells::HandlCastSpell);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new everbloom_spells();
        }
};

/// Toxic Gas Areatrigger - 169224
class the_everbloom_xeritac_toxic_gas : public AreaTriggerEntityScript
    {
    public:
        the_everbloom_xeritac_toxic_gas() : AreaTriggerEntityScript("the_everbloom_xeritac_toxic_gas") { }

        uint32 m_Diff = 2 * TimeConstants::IN_MILLISECONDS;
        uint32 m_DiffInhale = 6 * TimeConstants::IN_MILLISECONDS;

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
            /// Gas Damage
            if (m_Diff <= p_Time)
            {
                std::list<Player*> l_ListTargets;
                JadeCore::AnyPlayerInObjectRangeCheck l_Check(p_AreaTrigger, 1.5f);
                JadeCore::PlayerListSearcher<JadeCore::AnyPlayerInObjectRangeCheck> l_Searcher(p_AreaTrigger, l_ListTargets, l_Check);
                p_AreaTrigger->VisitNearbyObject(1.5f, l_Searcher);

                for (Player* l_Iter : l_ListTargets)
                {
                    if (!l_Iter->HasAura(eXeritacSpells::SpellToxicGasDamage))
                    {
                        l_Iter->AddAura(eXeritacSpells::SpellToxicGasDamage, l_Iter);

                        if (AuraPtr l_Aura = l_Iter->GetAura(eXeritacSpells::SpellToxicGasDamage))
                            l_Aura->SetDuration(1);
                    }
                }

                m_Diff = 2 * TimeConstants::IN_MILLISECONDS;;
            }
            else
                m_Diff -= p_Time;

            /// Inhale
            if (m_DiffInhale <= p_Time)
            {
                /// Pale Orc
                if (Creature* l_PaleOne = p_AreaTrigger->FindNearestCreature(eEverbloomCreatures::CreatureVenomCrazedPaleOne, 2.0f, true))
                {
                    if (l_PaleOne->GetAI())
                        l_PaleOne->GetAI()->DoAction(eXeritacActions::ActionInhale);
                }

                m_DiffInhale = 6 * TimeConstants::IN_MILLISECONDS;
            }
            else
                m_DiffInhale -= p_Time;
        }

        the_everbloom_xeritac_toxic_gas* GetAI() const override
        {
            return new the_everbloom_xeritac_toxic_gas();
        }
};

void AddSC_boss_xeritac()
{
    /// Boss
    new boss_xeritac();

    /// Npcs
    new the_everbloom_venom_sprayer();
    new the_everbloom_venom_crazed_pale_one();
    new the_everbloom_toxic_spiderling();
    new the_everbloom_gorged_bruster();
    new the_everbloom_xeritac_beam();
    new the_everbloom_trigger_gas();

    /// Spells
    new the_everbloom_descend_trigger();
    new the_everbloom_descend();
    new the_everbloom_gaseous_volley();
    new the_everbloom_toxic_blood_explosion();
    new the_everbloom_descend_damage_trigger();
    new the_everbloom_burst();
    new the_everbloom_consume();

    /// AreaTrigger
    new the_everbloom_xeritac_toxic_gas();
}