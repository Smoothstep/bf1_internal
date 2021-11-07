#include "stdafx.h" 
#include "Utility.h"
#include "D3DRenderer.h"

Vector4f VecFromColor(uint32_t Color)
{
	return Vector4f
	{
		static_cast<float>(static_cast<uint8_t>(Color >> 24)) / 255.0f,
		static_cast<float>(static_cast<uint8_t>(Color >> 16)) / 255.0f,
		static_cast<float>(static_cast<uint8_t>(Color >> 8)) / 255.0f,
		static_cast<float>(static_cast<uint8_t>(Color)) / 255.0f
	};
}

Vector4f VecFromColor(std::string_view Hex)
{
	if (Hex.size() != 9)
	{
		return {};
	}

	std::stringstream Ss;
	uint32_t Value;
	Ss << std::hex << &Hex[1];
	Ss >> Value;
	
	return VecFromColor(Value);
}

template<class T>
constexpr auto DefaultValue()
{
	return []() -> T { return {}; };
}

template<class T>
T ConfigConditional(const TConfigValue& Value, std::function<T()> Fallback = DefaultValue<T>())
{
	return std::visit([Fallback{ std::move(Fallback) }](const auto& Value)
	{
		using TAdapter = TypeAdapter<T>;
		using TValue = decltype(Value);

		if constexpr (std::is_convertible_v<TValue, TAdapter>)
		{
			return TAdapter { Value };
		}
		else
		{
			return std::invoke(Fallback);
		}
	}, Value);
}

float ConfigFloat(const TConfigValue& Value, const float Fallback = 0.f)
{
	return ConfigConditional<float>(Value, [Fallback]() { return Fallback; });
}

struct PValue
{
	float& ValueRef;

	constexpr PValue(float& ValueRef)
		: ValueRef{ ValueRef }
	{}

	float operator=(float Value)
	{
		return ValueRef = Value / 100.0f;
	}

	constexpr operator float() const
	{
		return ValueRef;
	}

	constexpr float operator()(float Value) const
	{
		return ValueRef = Value / 100.0f;
	}
};

Utility::Utility(D3D::GameInterface* Gi, GamePatcher* Patcher, const ValueRefMapWrapper<TValueMap>& ConfigMap)
	: Util(Patcher, Gi)
	, Misc(Patcher, Gi)
	, Esp(Patcher, Gi)
	, RcBehavior(GDefault)
	, SpBehavior()
	, Spotting(Patcher, Gi, SpBehavior)
	, Aimbot(RcBehavior, GDefault)
	, Triggerbot(RcBehavior, GDefault)
	, Aim(RcBehavior, Aimbot.GetBehavior(), Triggerbot.GetBehavior())
{
	// Game settings
	{
		ValueFnRefMap.emplace(L"game.noSpread", Misc.EnableNoSpread);
		ValueFnRefMap.emplace(L"game.noRecoil", Misc.EnableNoRecoil);
		ValueFnRefMap.emplace(L"game.noBreath", Misc.EnableNoBreath);
		ValueFnRefMap.emplace(L"game.thirdPerson", Misc.EnableThirdPerson);
		ValueFnRefMap.emplace(L"game.bipodAlwaysSet", Misc.BipodAlwaysSet);
		ValueFnRefMap.emplace(L"game.bipodEveryWeapon", Misc.BipodEveryWeapon);
		ValueFnRefMap.emplace(L"game.bipodEverywhere", Misc.BipodEverywhere);
	}

	// ESP settings
	{
		ValueFnRefMap.emplace(L"esp.x3DMap", Esp.Enable3DMap);
		ValueFnRefMap.emplace(L"esp.radar", Esp.EnableRadar);
		ValueFnRefMap.emplace(L"esp.outlines", Esp.EnableEngineOutlines);
		ValueFnRefMap.emplace(L"esp.nametags", Esp.EnableNametags);
		ValueFnRefMap.emplace(L"esp.indicators", Esp.EnableIndicators);
		ValueFnRefMap.emplace(L"esp.chams", Esp.EnableChams);
		ValueFnRefMap.emplace(L"esp.fadeDistance", Esp.FadeDistance);
		ValueFnRefMap.emplace(L"esp.fadeDistanceNames", Esp.FadeDistanceNames);
		ValueFnRefMap.emplace(L"esp.disableTerrainCulling", Esp.DisableTerrainCulling);
		ValueFnRefMap.emplace(L"esp.colors.enemy", Esp.ColorEnemy);
		ValueFnRefMap.emplace(L"esp.colors.enemyOccluded", Esp.ColorEnemyOccluded);
		ValueFnRefMap.emplace(L"esp.colors.allied", Esp.ColorFriendly);
		ValueFnRefMap.emplace(L"esp.colors.alliedOccluded", Esp.ColorFriendlyOccluded);
	}

	// Aim settings
	{
		ValueFnRefMap.emplace(L"aim.aimBones", Aim.AimBones);
		ValueFnRefMap.emplace(L"aim.targetBones", Aim.TriggerBone);
		ValueFnRefMap.emplace(L"aim.aimKey", [&](const int Character)
		{
			Aim.AimKey = VkKeyScan(Character);
		});
		ValueFnRefMap.emplace(L"aim.curves", Aim.AimCurves);
	}

	// Aimbot settings
	{
		AimBehavior& Behavior = Aimbot.GetBehavior();

		ValueFnRefMap.emplace(L"aim.x3DDistance", std::ref(Behavior.MaxDistanceWorld));
		ValueFnRefMap.emplace(L"aim.x2DDistance", std::ref(Behavior.MaxDistanceScreen));
		ValueFnRefMap.emplace(L"aim.velocity", std::ref(Behavior.Constants.AimVelocity));
		ValueFnRefMap.emplace(L"aim.accuracy", PValue{ Behavior.Constants.AimAccuracy });
		ValueFnRefMap.emplace(L"aim.smoothness", PValue{ Behavior.Constants.AimSmoothness });
		ValueFnRefMap.emplace(L"aim.variation", PValue{ Behavior.Constants.AimVariance });
		ValueFnRefMap.emplace(L"aim.weights.healthWeight", PValue{ Behavior.Weights.HealthDifferenceWeight });
		ValueFnRefMap.emplace(L"aim.weights.x3DDistanceWeight", PValue{ Behavior.Weights.D3DistanceImportanceWeight });
		ValueFnRefMap.emplace(L"aim.weights.x2DDistanceWeight", PValue{ Behavior.Weights.D2DistanceImportanceWeight });
		ValueFnRefMap.emplace(L"aim.weights.previousTargetWeight", PValue{ Behavior.Weights.PreviousTargetWeightMultiplier });
		ValueFnRefMap.emplace(L"aim.enableRaycast", std::ref(Behavior.UseRaycast));
		ValueFnRefMap.emplace(L"aim.silent", std::ref(Behavior.Silent));
		ValueFnRefMap.emplace(L"aim.targetSoldiers", Aim.EnableTargetSoldiers);
		ValueFnRefMap.emplace(L"aim.targetEnemies", Aim.EnableTargetEnemies);
		ValueFnRefMap.emplace(L"aim.targetAllies", Aim.EnableTargetAllies);
		ValueFnRefMap.emplace(L"aim.targetVehicles", Aim.EnableTargetVehicles);
	}

	// Triggerbot settings
	{
		TriggerBehavior& Behavior = Triggerbot.GetBehavior();

		ValueFnRefMap.emplace(L"trigger.x3DDistance", std::ref(Behavior.MaxDistanceWorld));
		ValueFnRefMap.emplace(L"trigger.preferredBone", std::ref(Behavior.TriggerBones));
	}

	// Util settings
	{
		ValueFnRefMap.emplace(L"util.enableAimbot", std::ref(Util.AimBot));
		ValueFnRefMap.emplace(L"util.enableTriggerbot", std::ref(Util.TriggerBot));
		ValueFnRefMap.emplace(L"util.screenshotPolicy", Util.ScreenshotPolicy);
		ValueFnRefMap.emplace(L"util.rawInput", Util.InputType);
	}

	// Spotting settings
	{
		ValueFnRefMap.emplace(L"spotting.noFOV", Spotting.SpottingNoFOV);
		ValueFnRefMap.emplace(L"spotting.autoSpot", Spotting.SpottingAlwaysActive);
		ValueFnRefMap.emplace(L"spotting.noOccluded", Spotting.SpottingOccluded);
		ValueFnRefMap.emplace(L"spotting.noVisibility", Spotting.SpottingVisibility);
		ValueFnRefMap.emplace(L"spotting.spotTeam", Spotting.SpottingTeam);
		ValueFnRefMap.emplace(L"spotting.spotBot", std::ref(Spotting.SpottingBot));
		ValueFnRefMap.emplace(L"spotting.spotDelay", std::ref(SpBehavior.Delay));
	}

	LoadConfig(ConfigMap);
}

ValueRefMapWrapper<TValueMap> DefaultSettings()
{
	ValueRefMapWrapper<TValueMap> Map{};
	{
		Map.emplace(L"esp.charms", true);
		Map.emplace(L"esp.indicators", true);
		Map.emplace(L"aim.aimKey", 0xDE);
		Map.emplace(L"util.aimBot", true);
		Map.emplace(L"game.noSpread", true);
		Map.emplace(L"esp.disableTerrainCulling", true);
		Map.emplace(L"spotting.spotBot", true);
		Map.emplace(L"game.thirdPerson", true);
	}

	return Map;
}

Utility::Utility(D3D::GameInterface* Gi, GamePatcher* Patcher, const Default&)
	: Utility(Gi, Patcher, DefaultSettings())
{}

void Utility::LoadConfig(const ValueRefMapWrapper<TValueMap>& ConfigMap)
{
	for (const auto& ConfigVal : ConfigMap)
	{
		if (auto Iter = ValueFnRefMap.find(ConfigVal.first))
		{
			Iter.value() = ConfigVal.second;
		}
	}
}

void Utility::ExecuteKeyAction(InputController& Ic, uint16_t Key, bool Up)
{
	if (Util.AimBot)
	{
		if (Key == Aim.AimKey && Up)
		{
			Aimbot.Stop(Ic);
		}
	}
}

void Utility::ExecuteRoutine(InputController& Ic)
{
	if (Spotting.SpottingBot)
	{
		AutoSpot.Execute(SpBehavior);
	}

	if (Util.AimBot)
	{
		if ((GetAsyncKeyState(Aim.AimKey) & 0x8000) != 0)
		{
			Aimbot.Execute(Ic);
		}
	}

	if (Util.TriggerBot)
	{
		Triggerbot.Evaluate(Ic);
	}
}

MiscSettings::MiscSettings(GamePatcher* Patcher, D3D::GameInterface* Gi)
	: EnableNoSpread([=](ToggleState Value)
	{
		Patcher->ToggleNoSpread(Value);
	})
	, EnableNoBreath([=](ToggleState Value)
	{
		Patcher->ToggleNoBreath(Value);
	})
	, EnableNoRecoil([=](ToggleState Value)
	{
		Patcher->ToggleNoRecoil(Value);
	})
	, EnableThirdPerson([Patcher, Gi](ToggleState Value)
	{
		Gi->GetInputCtrl().SetThirdPersonMode(Value);
		Patcher->ToggleThirdPerson(Value);
	})
	, BipodAlwaysSet([=](ToggleState Value)
	{
		Patcher->ToggleBipodAlwaysSet(Value);
	})
	, BipodEverywhere([=](ToggleState Value)
	{
		Patcher->ToggleBipodEverywhere(Value);
	})
	, BipodEveryWeapon([=](ToggleState Value)
	{
		Patcher->ToggleBipodAllWeapons(Value);
	})
{}

SpottingSettings::SpottingSettings(GamePatcher* Patcher, D3D::GameInterface* Interface, SpottingBehavior& Behavior)
	: SpottingAlwaysActive([=](ToggleState State)
	{
		Patcher->ToggleSpottingAlwaysActive(State);
	})
	, SpottingNoFOV([=](ToggleState State)
	{
		Patcher->ToggleSpottingFOV(State);
	})
	, SpottingVisibility([=](ToggleState State)
	{
		Patcher->ToggleSpottingVisiblity(State);
	})
	, SpottingTeam([=](ToggleState State)
	{
		Patcher->ToggleSpottingTeam(State);
	})
	, SpottingOccluded([=](ToggleState State)
	{
		Patcher->ToggleSpottingOccluded(State);
	})
	, SpottingBot(false)
{}

ESPSettings::ESPSettings(GamePatcher* Patcher, D3D::GameInterface* Interface)
	: Enable3DMap([=](ToggleState State)
	{
		Patcher->Toggle3DEsp(State);
	})
	, EnableRadar([=](ToggleState State)
	{
		Patcher->ToggleRadar(State);
	})
	, EnableNametags([=](ToggleState State)
	{
		Interface->ToggleNameTags(State);
	})
	, EnableIndicators([=](ToggleState State)
	{
		Interface->ToggleBBoxes(State);
	})
	, EnableEngineOutlines([=](ToggleState State)
	{
		Interface->ToggleEngineChams(State);
	})
	, EnableChams([=](ToggleState State)
	{
		Interface->ToggleXRay(State);
	})
	, FadeDistance([=](float Value)
	{
		if (auto* Renderer = Interface->GetRenderer())
		{
			Renderer->GShaderConstants().FadeDistanceIndicators = Value;
		}
	})
	, FadeDistanceNames([=](float Value)
	{
		if (auto* Renderer = Interface->GetRenderer())
		{
			Renderer->GShaderConstants().FadeDistanceNameTags = Value;
		}
	})
	, ColorFriendly([=](std::string_view Value)
	{
		if (auto* Renderer = Interface->GetRenderer())
		{
			Renderer->GShaderConstants().ColorAlly = VecFromColor(Value);
		}
	})
	, ColorFriendlyOccluded([=](std::string_view Value)
	{
		if (auto* Renderer = Interface->GetRenderer())
		{
			Renderer->GShaderConstants().ColorAllyOccluded = VecFromColor(Value);
		}
	})
	, ColorEnemy([=](std::string_view Value)
	{
		if (auto* Renderer = Interface->GetRenderer())
		{
			Renderer->GShaderConstants().ColorEnemy = VecFromColor(Value);
		}
	})
	, ColorEnemyOccluded([=](std::string_view Value)
	{
		if (auto* Renderer = Interface->GetRenderer())
		{
			Renderer->GShaderConstants().ColorEnemyOccluded = VecFromColor(Value);
		}
	})
	, DisableTerrainCulling([=](ToggleState State)
	{
		Patcher->ToggleTerrainCulling(State);
	})
{}

UtilitySettings::UtilitySettings(GamePatcher* Patcher, D3D::GameInterface * Interface)
	: InputType([=](EInputType InputType)
	{
		Interface->GetInputCtrl().SetInputType(InputType);
	})
	, ScreenshotPolicy([=](std::string_view Policy)
	{
		const auto Policies = std::unordered_map<std::string_view, EScreenshotPolicy>(
			{ 
				{ "None", EScreenshotPolicy::DoNothing },
				{ "Blacken", EScreenshotPolicy::PolicyBlacken },
				{ "Clean", EScreenshotPolicy::PolicyClean },
				{ "Ignore", EScreenshotPolicy::PolicyIgnore },
			}
		);

		Interface->GetCleaner().SetPolicy(Policies.at(Policy));
	})
	, AimBot(false)
	, TriggerBot(false)
{}

AimSettings::AimSettings(RaycastBehavior& RcBehavior, AimBehavior& AimBehavior, TriggerBehavior& TriggerBehavior)
	: AimBones([&](const std::vector<std::string>& Values)
	{
		AimBehavior.TargetBones.clear();

		for (auto& Val : Values)
		{
			if (auto Iter = BoneIdentMap.find(Val); Iter != BoneIdentMap.end())
			{
				AimBehavior.TargetBones.push_back(Iter->second);
			}
		}
	})
	, TriggerBone([&](const std::vector<std::string>& Values)
	{
		TriggerBehavior.TriggerBones.clear();

		for (auto& Val : Values)
		{
			if (auto Iter = BoneIdentMap.find(Val); Iter != BoneIdentMap.end())
			{
				TriggerBehavior.TriggerBones.push_back(Iter->second);
			}
		}
	})
	, AimCurves([&](const std::vector<std::unordered_map<std::string, TConfigValue>>& Curves)
	{
		decltype(AimBehavior.AimCurves) NewCurves;
		
		try
		{
			for (const auto& CurveObj : Curves)
			{
				if (const auto* PointObj = std::get_if<std::vector<TConfigValue>>(&CurveObj.at("points")))
				{
					NewCurves.push_back(
						{
							std::make_shared<AimCurve>(TypeAdapter<std::vector<float>>{ *PointObj }, ConfigFloat(CurveObj.at("avg"))),
							{
								ConfigConditional<std::array<float, 2>>(CurveObj.at("pixelDistance")),
								ConfigFloat(CurveObj.at("usageProbability")),
								CurveOvershootAttributes
								{
									ConfigFloat(CurveObj.at("overshootFrequency")),
									ConfigFloat(CurveObj.at("overshootFactor")),
									ConfigFloat(CurveObj.at("overshootDegradation")),
									ConfigFloat(CurveObj.at("overshootMeanX")),
									ConfigFloat(CurveObj.at("overshootSigmaX")),
									ConfigFloat(CurveObj.at("overshootMeanY")),
									ConfigFloat(CurveObj.at("overshootSigmaY")),
								}
							}
						});
				}
			}

			AimBehavior.AimCurves = NewCurves;
		}
		catch (const std::out_of_range& Oor)
		{
			Logger::Error() << "Illegal AimCurve object: " << Oor.what() << Logger::EndLine;
		}
		catch (const std::bad_variant_access& Bva)
		{
			Logger::Error() << "Illegal AimCurve object field type: " << Bva.what() << Logger::EndLine;
		}

	})
	, RaycastBones([&](const std::vector<std::string>& Values)
	{
		RcBehavior.Bones.clear();

		for (auto& Val : Values)
		{
			if (auto Iter = BoneIdentMap.find(Val); Iter != BoneIdentMap.end())
			{
				RcBehavior.Bones.push_back(Iter->second);
			}
		}
	})
	, EnableTargetAllies([&](const bool On)
	{
		if (On)
		{
			AimBehavior.Targets |= TargetAllies;
		}
		else
		{
			AimBehavior.Targets &= ~TargetAllies;
		}
	})
	, EnableTargetEnemies([&](const bool On)
	{
		if (On)
		{
			AimBehavior.Targets |= TargetEnemies;
		}
		else
		{
			AimBehavior.Targets &= ~TargetEnemies;
		}
	})
	, EnableTargetSoldiers([&](const bool On)
	{
		if (On)
		{
			AimBehavior.Targets |= TargetSoldiers;
		}
		else
		{
			AimBehavior.Targets &= ~TargetSoldiers;
		}
	})
	, EnableTargetVehicles([&](const bool On)
	{
		if (On)
		{
			AimBehavior.Targets |= TargetVehicles;
		}
		else
		{
			AimBehavior.Targets &= ~TargetVehicles;
		}
	})
	, AimKey{ -1 }
{}