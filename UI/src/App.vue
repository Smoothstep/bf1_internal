<template>
	<v-app>
		<v-main>
			<v-navigation-drawer fill-height id="snav" class="NavWindow en col s4" v-model="drawer" color="transparent" width="250" :floating=true>
				<v-layout row wrap align-center justify-center>
				</v-layout>
				<v-container fill-height fluid>
					<v-row align="center" justify="center">
						<v-col>
							
							<ul>
								<li v-for="item in menuItems" :key="item" @click="currentMenu = item">
									<a> 
										<v-icon>{{ item.icon }}</v-icon>
										<span>{{ item.title }}</span> 
									</a>
								</li>
							</ul>
						</v-col>
					</v-row>
				</v-container>
			</v-navigation-drawer>
			<v-container fluid fill-height justify="center" class="overflow-y-auto">
				<v-row cols="1" align="center" class="mx-auto">
				<v-slide-y-transition>
					
						
						<v-card :width="700" :elevation="10" class="acrylic mx-auto" v-show="currentMenu == menuItems['esp']" :key="menuItems['esp']">
							<v-toolbar fixed-tabs dark flat class="header">
								<v-toolbar-title>ESP settings</v-toolbar-title>
								<v-spacer />
								<v-btn icon hint="Close" @click="currentMenu = null">
									<v-icon>mdi-window-close</v-icon>
								</v-btn>
								<template v-slot:extension>
									<v-tabs align-with-title v-model="espTabs">
										<v-tab href="#esp-general">General</v-tab>
										<v-tab href="#esp-misc">Misc</v-tab>
										<v-tabs-slider color="primary"/>
									</v-tabs>
								</template>
							</v-toolbar>
							
							<v-tabs-items v-model="espTabs" class="container-bg">
								<v-tab-item value="esp-misc">
									<v-card-text>
										<v-form>
											<v-switch persistent-hint="true" hint="Slightly affects performance" class="ma-1" label="Ignore Terrain occlusion check" v-model="esp.disableTerrainCulling" />
											<v-subheader>Fade distance</v-subheader>
											<v-card-text>
												<v-slider thumb-label :min="0" :max="10000" prepend-icon="mdi-eye" />
											</v-card-text>
										</v-form>
									</v-card-text>
								</v-tab-item>
								<v-tab-item value="esp-general">
									<v-card-text>
										<v-form>
											<v-switch class="ma-1" label="Enable Nametags" v-model="esp.nametags" />
											
											<v-switch class="ma-1" label="Enable Indicators" v-model="esp.indicators" />
											<v-switch class="ma-1" label="Enable Radar" v-model="esp.radar" />
											
											
											<v-switch class="ma-1" label="Enable Chams" v-model="esp.chams" />
											<v-switch class="ma-1" label="Enable engine outlines" v-model="esp.outlines" />

											<v-sheet :rounded=true :elevation="0" :outlined=true hide-actions color="rgba(0,0,0,0.2)">
												<v-expansion-panels flat>
													<v-expansion-panel  class="sheet">
														<v-expansion-panel-header>Color settings</v-expansion-panel-header>
								
														
														<v-expansion-panel-content>
															<v-divider/>
													
															<v-container>
																<v-row>
																	<v-col justify-center class="mx-auto text-md-center">
																		<v-header justify-center>Allied Colors</v-header>
																	</v-col>
																	<v-col justify-center class="mx-auto text-md-center">
																		<v-header>Occluded Allied Colors</v-header>
																	</v-col>
																</v-row>
																<v-row>
																	<v-color-picker
																		dot-size="25"
																		mode="rgba"
																		v-model="esp.colors.allied"
																	></v-color-picker>
																	<v-spacer />
																	<v-color-picker
																		dot-size="25"
																		mode="rgba"
																		v-model="esp.colors.alliedOccluded"
																	></v-color-picker>
																</v-row>
																<v-row>
																	<v-col justify-center class="mx-auto text-md-center">
																		<v-header justify-center>Enemy Colors</v-header>
																	</v-col>
																	<v-col justify-center class="mx-auto text-md-center">
																		<v-header>Occluded Enemy Colors</v-header>
																	</v-col>
																</v-row>
																<v-row>
																	<v-color-picker
																		dot-size="25"
																		mode="rgba"
																		v-model="esp.colors.enemy"
																	></v-color-picker>
																	<v-spacer />
																	<v-color-picker
																		dot-size="25"
																		mode="rgba"
																		v-model="esp.colors.enemyOccluded"
																	></v-color-picker>
																</v-row>
															</v-container>
														</v-expansion-panel-content>
													</v-expansion-panel>
												</v-expansion-panels>
											</v-sheet>
										</v-form>
									</v-card-text>
								</v-tab-item>
							</v-tabs-items>
						</v-card>
					</v-slide-y-transition>
					<v-slide-y-transition>
						<v-card :width="700" :elevation="0" position="absolute" :left="0" :right="0" class="acrylic mx-auto"  
								v-show="currentMenu == menuItems['safety']" :key="menuItems['safety']">
							<v-toolbar fixed-tabs dark flat class="header">
								<v-toolbar-title>Safety settings</v-toolbar-title>
								<v-spacer />
								<v-btn icon hint="Close" @click="currentMenu = null">
									<v-icon>mdi-window-close</v-icon>
								</v-btn>
								<template v-slot:extension>
									<v-tabs align-with-title v-model="safetyTabs">
										<v-tab href="#safety-general">General</v-tab>
										<v-tabs-slider color="primary"/>
									</v-tabs>
								</template>
							</v-toolbar>
							
							<v-tabs-items v-model="safetyTabs" class="container-bg">
								<v-tab-item value="safety-general">
									<v-card-text>
										<v-form>
											<v-select outlined persistent-hint :items="screenshotPolicies" v-model="util.screenshotPolicy" label="Screenshots" />
										</v-form>
									</v-card-text>
								</v-tab-item>
							</v-tabs-items>
						</v-card>
					</v-slide-y-transition>
					<v-slide-y-transition>
						<v-card :width="700" :elevation="0" position="absolute" :left="0" :right="0" class="acrylic mx-auto"  
								v-show="currentMenu == menuItems['settings']" :key="menuItems['settings']">
							<v-toolbar fixed-tabs dark flat class="header">
								<v-toolbar-title>Settings</v-toolbar-title>
								<v-spacer />
								<v-btn icon hint="Close" @click="currentMenu = null">
									<v-icon>mdi-window-close</v-icon>
								</v-btn>
								<template v-slot:extension>
									<v-tabs align-with-title v-model="settingsTabs">
										<v-tab href="#settings-general">General</v-tab>
										<v-tabs-slider color="primary"/>
									</v-tabs>
								</template>
							</v-toolbar>
							
							<v-tabs-items v-model="settingsTabs" class="container-bg">
								<v-tab-item value="settings-general">
									<v-card-text>
										<v-form>
											<v-sheet :rounded=true :elevation="0" :outlined=true hide-actions color="rgba(0,0,0,0.0)">
											<v-row>
												<v-dialog v-model="showSaveDialog" width="500">
													<v-card>
														<v-card-title class="text-h5">Save config as</v-card-title>
														<v-card-text>
															<v-text-field outlined label="Config name" v-model="configName" />
														</v-card-text>
														<v-card-actions>
															<v-spacer />
															<v-btn text @click="showSaveDialog = false">Cancel</v-btn>
															<v-btn text color="green darken-1" @click="saveConfig(configName)">Save</v-btn>
														</v-card-actions>
													</v-card>
												</v-dialog>
												<v-dialog v-model="showLoadDialog" width="500">
													<v-card>
														<v-card-title class="text-h5">Load config file</v-card-title>
														<v-card-text>
															<v-select outlined label="Config name" v-model="loadConfigName" :items="configList">
																<template v-slot:selection="{ item }">
																	<span>{{ item.file }}</span>
																</template>
																<template v-slot:item="{ item }">
																	<span>{{ item.file }}</span>
																</template>
															</v-select>
														</v-card-text>
														<v-card-actions>
															<v-spacer />
															<v-btn text @click="showLoadDialog = false">Cancel</v-btn>
															<v-btn text color="green darken-1" @click="loadConfig(loadConfigName)">Load</v-btn>
														</v-card-actions>
													</v-card>
												</v-dialog>
												<v-snackbar :timeout="3000" v-model="showSaveSnackbarSuccess">Save succeeded</v-snackbar>
												<v-snackbar :timeout="3000" v-model="showSaveSnackbarFail">
													Save failed
													<template v-slot:action="{ attrs }">
														<v-btn color="pink" text v-bind="attrs" @click="showSaveSnackbarFail = false">Close</v-btn>
													</template>
												</v-snackbar>
												<v-snackbar :timeout="3000" v-model="showLoadSnackbarSuccess">Load succeeded</v-snackbar>
												<v-snackbar :timeout="3000" v-model="showLoadSnackbarFail">
													Load failed
													<template v-slot:action="{ attrs }">
														<v-btn color="pink" text v-bind="attrs" @click="showLoadSnackbarFail = false">Close</v-btn>
													</template>
												</v-snackbar>
												<v-spacer />
												<v-col>
													<div class="my-2">
														<v-btn tile color="primary darken-1" large width="300" @click="showLoadDialog = true">
															<v-icon left>mdi-content-save</v-icon>
															Load settings
														</v-btn>
													</div>
													<div class="my-2">
														<v-btn tile color="primary darken-1" large width="300" @click="showSaveDialog = true">
															<v-icon left>mdi-content-save</v-icon>
															Save current settings
														</v-btn>
													</div>
												</v-col>
												<v-spacer />
												<v-col>
													<div class="my-2">
														<v-btn tile color="orange darken-2" large width="300" @click="loadConfig('default.json')">
															<v-icon left>mdi-content-save</v-icon>
															Restore default settings
															</v-btn>
													</div>
													<div class="my-2">
														<v-btn tile width="300" color="red darken-2" large @click="deleteConfig(configName)">
															<v-icon left>mdi-content-save</v-icon>
															Delete current settings
														</v-btn>
													</div>
												</v-col>
												<v-spacer />
											</v-row>
											</v-sheet>
										</v-form>
									</v-card-text>
								</v-tab-item>
							</v-tabs-items>
						</v-card>
					</v-slide-y-transition>
					<v-slide-y-transition>
						<v-card :width="700" :elevation="0" position="absolute" :left="0" :right="0" class="acrylic mx-auto"  
								v-show="currentMenu == menuItems['spotting']" :key="menuItems['spotting']">
							<v-toolbar fixed-tabs dark flat class="header">
								<v-toolbar-title>Spotting settings</v-toolbar-title>
								<v-spacer />
								<v-btn icon hint="Close" @click="currentMenu = null">
									<v-icon>mdi-window-close</v-icon>
								</v-btn>
								<template v-slot:extension>
									<v-tabs align-with-title v-model="spottingTabs">
										<v-tab href="#spotting-general">General</v-tab>
										<v-tabs-slider color="primary"/>
									</v-tabs>
								</template>
							</v-toolbar>
							
							<v-tabs-items v-model="spottingTabs" class="container-bg">
								<v-tab-item value="spotting-general">
									<v-card-text>
										<v-form>
											<v-switch class="ma-1" label="Enable autospot" v-model="spotting.autoSpot" />
											<v-switch class="ma-1" label="Enable spotting bot" v-model="spotting.spotBot" />
											<v-switch class="ma-1" label="Disable FoV checks" v-model="spotting.noFOV" />
											<v-switch class="ma-1" label="Disable occlusion checks" v-model="spotting.noOccluded" />
											<v-switch class="ma-1" label="Disable visibility checks" v-model="spotting.noVisibility" />
											<v-switch class="ma-1" label="Disable team checks" v-model="spotting.spotTeam" />
											<v-switch class="ma-1" label="Disable team checks" v-model="spotting.spotTeam" />
										</v-form>
									</v-card-text>
								</v-tab-item>
							</v-tabs-items>
						</v-card>
					</v-slide-y-transition>
					<v-slide-y-transition>
						<v-card :width="700" :elevation="0" position="absolute" :left="0" :right="0" class="acrylic mx-auto"  
								v-show="currentMenu == menuItems['misc']" :key="menuItems['misc']">
							<v-toolbar fixed-tabs dark flat class="header">
								<v-toolbar-title>Misc settings</v-toolbar-title>
								<v-spacer />
								<v-btn icon hint="Close" @click="currentMenu = null">
									<v-icon>mdi-window-close</v-icon>
								</v-btn>
								<template v-slot:extension>
									<v-tabs align-with-title v-model="miscTabs">
										<v-tab href="#misc-general">General</v-tab>
										<v-tabs-slider color="primary"/>
									</v-tabs>
								</template>
							</v-toolbar>
							
							<v-tabs-items v-model="miscTabs" class="container-bg">
							<v-tab-item value="misc-general">
								<v-card-text>
									<v-form>
										<div class="v-heading text-h6 mb-2 font-weight-light heading">View model</div>
										<v-radio-group v-model="viewModel" class="mb-2">
											<v-radio label="First person" dense />
											<v-radio label="Third person" />
										</v-radio-group>
										<v-row cols="12" sm="6" no-gutters>
											<v-col md="5">
												<div class="v-heading text-h6 mb-2 font-weight-light heading">Weapon control</div>
											</v-col>
											<v-col md="2">
											</v-col>
											<v-col md="5">
												<div class="v-heading text-h6 mb-2 font-weight-light heading">Bipod</div>
											</v-col>
										</v-row>
										<v-row cols="12" sm="6" no-gutters>
											<v-col md="5">
												<v-switch persistent-hint class="ma-1" hint="Maybe unsafe" label="No recoil" v-model="game.noRecoil" />
												<v-switch persistent-hint class="ma-1" hint="Maybe unsafe" label="No spread" v-model="game.noSpread" />
												<v-switch persistent-hint class="ma-1" hint="" label="No breath" v-model="game.noBreath" />
											</v-col>
											<v-col md="2">
											</v-col>
											<v-col md="5">
												<v-switch class="ma-1" label="Always set" v-model="game.bipodAlwaysSet" />
												<v-switch class="ma-1" label="Enable on every weapon" v-model="game.bipodEveryWeapon" />
												<v-switch class="ma-1" label="Enable everywhere" v-model="game.bipodEverywhere" />
											</v-col>
										</v-row>
									</v-form>
								</v-card-text>
							</v-tab-item>
							</v-tabs-items>
						</v-card>
					</v-slide-y-transition>
					<v-slide-y-transition>
						<v-card :width="700" :elevation="0" position="absolute" :left="0" :right="0" class="acrylic mx-auto"  
								v-show="currentMenu == menuItems['aim']" :key="menuItems['aim']">
							<v-toolbar fixed-tabs dark flat class="header">
								<v-toolbar-title>Aim settings</v-toolbar-title>
								<v-spacer />
								<v-btn icon hint="Close" @click="currentMenu = null">
									<v-icon>mdi-window-close</v-icon>
								</v-btn>
								<template v-slot:extension>
									<v-tabs align-with-title v-model="aimTabs" slider-color="primary">
										<v-tab href="#aim-general">General</v-tab>
										<v-tab href="#aim-values">Values</v-tab>
										<v-tab href="#aim-misc">Misc</v-tab>
									</v-tabs>
								</template>
							</v-toolbar>
							
							<v-tabs-items v-model="aimTabs" class="container-bg">
							<v-tab-item value="aim-values">
								<v-card-text>
									<v-form>
										<v-header>Distance Threshold</v-header>
										<v-row cols="12" sm="6">
											<v-col>
												<v-subheader>Max 3D distance <v-spacer />(m)</v-subheader>
												<v-card-text>
													<v-range-slider min="0" max="10000" dense thumb-label v-model="aim.x3DDistance" />
												</v-card-text>
											</v-col>
											<v-col>
												<v-subheader>Max 2D distance</v-subheader>
												<v-card-text>
													<v-slider min="0" max="100" dense thumb-label v-model="aim.x2DDistance" />
												</v-card-text>
											</v-col>
										</v-row>
										<v-header>Target Weights</v-header>
										<v-row>
											<v-col>
												<v-subheader>Previous target weight</v-subheader>
												<v-card-text>
													<v-slider v-model="aim.weights.previous" prepend-icon="mdi-weight" min="0" max="100" thumb-label dense></v-slider>
												</v-card-text>
											</v-col>
											<v-col>
												<v-subheader>Health difference weight</v-subheader>
												<v-card-text>
													<v-slider v-model="aim.weights.health" prepend-icon="mdi-weight" min="0" max="100" thumb-label dense></v-slider>
												</v-card-text>
											</v-col>
										</v-row>
										<v-row>
											<v-col>
												<v-subheader>2D distance weight</v-subheader>
												<v-card-text>
													<v-slider v-model="aim.weights.distance2d" prepend-icon="mdi-weight" min="0" max="100" thumb-label dense></v-slider>
												</v-card-text>
											</v-col>
											<v-col>
												<v-subheader>3D distance weight</v-subheader>
												<v-card-text>
													<v-slider v-model="aim.weights.distance3d" prepend-icon="mdi-weight" min="0" max="100" thumb-label dense></v-slider>
												</v-card-text>
											</v-col>
										</v-row>
										<v-spacer />
										
									</v-form>
								</v-card-text>
							</v-tab-item>
							
							<v-tab-item value="aim-misc">
								<v-card-text>
									<v-form>
										<v-header>Input Type</v-header>
										<v-radio-group v-model="util.inputType">
											<v-radio label="Direct" dense />
											<v-subheader class="mt-n3">Applies euler angles directly to your view (maybe detected)</v-subheader>
											<v-radio label="Raw Input (Safe)" />
											<v-subheader class="mt-n3">Imitates mouse movement (normalized, less accurate)</v-subheader>
										</v-radio-group>
									</v-form>
								</v-card-text>
							</v-tab-item>
							
							<v-tab-item value="aim-general">
								<v-card-text class="overflow-y-auto">
									<v-form>
										<v-switch class="ma-1" label="Enable Aimbot" v-model="util.enableAimbot" />
										<v-switch class="ma-1" label="Enable Triggerbot" v-model="util.enableTriggerbot" />
										<v-switch v-model="aim.enableRaycast" class="ma-1" label="Enable Raycast" />
										<v-select chips multiple outlined persistent-hint v-visible="aim.enableRaycast" :items="aimBones" v-model="aim.raycastBones" hint="Multiple raycast bones affect performance" label="Raycast bones" />

										<v-spacer />
										
										<v-row>
											<v-col>
											
												<v-sheet :rounded=true :elevation="0" :outlined=true hide-actions color="rgba(0,0,0,0.2)">
												
													<v-expansion-panels flat :disabled="!util.enableAimbot">
														<v-expansion-panel color="#115e9040" class="sheet" ref="#panel">
															<v-expansion-panel-header>Aimbot settings</v-expansion-panel-header>
															<v-expansion-panel-content>
																<v-divider class="mal-n8" />
																<v-container color="#115e9040">
																	<v-row cols="12" sm="6">
																		<v-col>
																			<v-switch v-model="aim.silent" class="ma-1" label="Enable silent aim (risky)" />
																			<v-row>
																				<v-col>
																					<v-header>Target entity when</v-header>
																					<v-checkbox label="Same Team" v-model="aim.targetAllies" hide-details />
																					<v-checkbox label="Different Team" v-model="aim.targetEnemies" hide-details />
																					<v-checkbox label="Vehicle" v-model="aim.targetVehicles" hide-details />
																					<v-checkbox label="Soldier" v-model="aim.targetSoldiers" />
																				</v-col>
																			</v-row>
																			
																			<v-select outlined label="Preferred aimbone" :items="aimBones" v-model="aim.preferredBone"></v-select>
																			<v-text-field outlined label="Aimkey" @keyup="setAimkey" v-model="aim.aimKey" readonly=true></v-text-field>
																			
																			<v-subheader>Aim speed <v-spacer />0% - 100%</v-subheader>
																			<v-slider min="0" max="100" v-model="aim.velocity" thumb-label></v-slider>
																			<v-subheader>Aim accuracy <v-spacer />0% - 100%</v-subheader>
																			<v-slider min="0" max="100" v-model="aim.accuracy" thumb-label></v-slider>
																			<v-subheader>Curve smoothing <v-spacer />0px - 100px</v-subheader>
																			<v-slider min="0" max="100" v-model="aim.curveSmoothing" thumb-label></v-slider>
																			<v-subheader>Curve variation <v-spacer />0% - 100%</v-subheader>
																			<v-slider min="0" max="100" v-model="aim.curveVariation" thumb-label></v-slider>
																			<v-subheader>Curve</v-subheader>
																			<v-carousel height="auto" hide-delimiter-background v-model="aim.selectedCurve">
																				<v-carousel-item v-for="(item, index) in aim.curves" :key="index">
																					<v-container>
																						<v-sparkline :value="aim.curves[index].points"
																									height="200"
																									:smooth="aim.curveSmoothing"
																									:line-width="1"
																									:stroke-linecap="5"
																									:gradient-direction="gradientDirection"
																									:auto-line-width="autoLineWidth"
																									auto-draw></v-sparkline>
																						<v-row >
																							<v-col>
																								<v-subheader>Pixel Distance <v-spacer />0px - 5000px</v-subheader>
																								<v-range-slider dense min="0" max="5000" v-model="aim.curves[index].pixelDistance" thumb-label></v-range-slider>
																								<v-subheader>Usage Probability <v-spacer />0% - 100%</v-subheader>
																								<v-slider dense min="0" max="100" v-model="aim.curves[index].usageProbability" thumb-label class="mb-4"></v-slider>
																								<v-btn color="primary mb-10" @click="aim.curves[index].showOvershootDialog = true">Configure overshoot</v-btn>
																								<v-dialog v-model="aim.curves[index].showOvershootDialog" width="500">
																									<v-card>
																										<v-card-title class="headline">
																											Overshoot configuration
																											<v-spacer />
																											<v-btn icon @click.stop="aim.curves[index].showOvershootDialog = false">
																												<v-icon>mdi-window-close</v-icon>
																											</v-btn>
																										</v-card-title>
																										<v-card-text>
																											<v-text-field v-model="aim.curves[index].overshootFrequency" type="number" outlined label="Frequency" />
																											<v-text-field v-model="aim.curves[index].overshootFactor" type="number" outlined label="Factor" />
																											<v-text-field v-model="aim.curves[index].overshootDegradation" type="number" outlined label="Degradation over time" />
																											<v-subheader class="subtitle">X Evaluation</v-subheader>
																											<v-text-field v-model="aim.curves[index].overshootMeanX" type="number" outlined label="Mean" />
																											<v-text-field v-model="aim.curves[index].overshootSigmaX" type="number" outlined label="Sigma" />
																											<v-subheader>Y Evaluation</v-subheader>
																											<v-text-field v-model="aim.curves[index].overshootMeanY" type="number" outlined label="Mean" />
																											<v-text-field v-model="aim.curves[index].overshootSigmaY" type="number" outlined label="Sigma" />
																										</v-card-text>
																									</v-card>
																								</v-dialog>
																							</v-col>
																						</v-row>
																					</v-container>
																				</v-carousel-item>
																			</v-carousel>
																			<v-row cols="3" md="1">
																				<v-col />
																				<v-spacer />
																				<v-spacer />
																				<v-col>
																					<v-btn outlined color="error" right @click.stop="removeSelectedCurve" v-visible="aim.selectedCurve >= 0">
																						<v-icon left>mdi-delete</v-icon> Remove
																					</v-btn>
																				</v-col>
																				<v-col>
																					<v-btn outlined color="success" right @click.stop="showCurveDialog = true">
																						<v-icon left>mdi-pencil</v-icon> New
																					</v-btn>
																				</v-col>
																			</v-row>
																		</v-col>
																	</v-row>
																</v-container>
															</v-expansion-panel-content>
														</v-expansion-panel>
													</v-expansion-panels>
												</v-sheet>
											</v-col>
										</v-row>
										
										<v-spacer />
										<v-row>
											<v-col>
												<v-sheet :rounded=true :elevation="0" :outlined=true hide-actions color="rgba(0,0,0,0.2)">
													<v-expansion-panels :disabled="!util.enableTriggerbot" flat>
														<v-expansion-panel class="sheet">
															<v-expansion-panel-header>Triggerbot settings</v-expansion-panel-header>


															<v-expansion-panel-content>
																<v-divider />

																<v-container>
																	<v-row cols="12" sm="6">
																		<v-col>
																			<v-select outlined label="Aimbone" :items="aimBones" v-model="trigger.preferredBone" />
																			<v-subheader>3D distance</v-subheader>
																			<v-range-slider min="0" max="10000" thumb-label v-model="trigger.x3DDistance" />
																			<v-subheader>2D distance</v-subheader>
																			<v-slider min="0" max="100" thumb-label v-model="trigger.x2DDistance" />
																		</v-col>
																	</v-row>
																</v-container>
															</v-expansion-panel-content>
														</v-expansion-panel>
													</v-expansion-panels>
												</v-sheet>
											</v-col>
										</v-row>
									</v-form>
								</v-card-text>
							</v-tab-item>
							</v-tabs-items>
						</v-card>
					</v-slide-y-transition>
					
				</v-row>
			</v-container>
			<v-dialog v-model="showCurveDialog" id="create-curve" width="450">
				<v-card>
					<v-card-title class="headline">
						Add a new curve
						<v-spacer />
						<v-btn icon @click.stop="showCurveDialog = false">
							<v-icon>mdi-window-close</v-icon>
						</v-btn>
					</v-card-title>
					<v-card-text>
						This action will add a new aim curve as one possible path to evaluate the angles when executing the aimbot.
					</v-card-text>
					<v-card-actions>
						<v-row cols="1" md="2">
							<v-col>
								<v-btn @click.stop="drawCurveOverlay = true; showCurveDialog = false">
									Draw a curve
								</v-btn>
							</v-col>
						</v-row>
						<v-spacer />
						<v-row cols="1" md="2">
							<v-col>
								<v-btn>
									From mouse movement
								</v-btn>
							</v-col>
							<v-spacer />
						</v-row>
					</v-card-actions>
				</v-card>
			</v-dialog>
			<v-overlay :value="drawCurveOverlay">
				<v-card>
					<v-card-title>
						Paint a curve...
						<v-spacer />
						<v-btn icon @click.stop="saveCurve">
							<v-icon>mdi-check</v-icon>
						</v-btn>
						<v-btn icon @click.stop="resetCurve">
							<v-icon>mdi-undo</v-icon>
						</v-btn>
						<v-btn icon @click.stop="drawCurveOverlay = false">
							<v-icon>mdi-window-close</v-icon>
						</v-btn>
					</v-card-title>
					<v-card-text>
						<canvas ref="curve-canvas" @mousemove="drawCurveMouseOver" width="1024" height="600" style="border: 1px solid gray; cursor: crosshair;" />
					</v-card-text>
				</v-card>
			</v-overlay>
		</v-main>
	</v-app>
</template>

<script>
	import { genPath } from 'vuetify/lib/components/VSparkline/helpers/path'
	
	const DataKeys = ['aim', 'esp', 'trigger', 'util', 'game'];
	const ConfigListFile = "configs.json";
	
	export default {
		props: {
			source: String,
		},
		methods: {
			setAndRestore: function(varName, value, time) {
				let original = this[varName];
				if (original == value) {
					return;
				}
				this[varName] = value;
				setTimeout(() => {
					this[varName] = original;
				}, time);
			},
			deleteConfig: function(file) {
				if (!file) {
					return;
				}
				try {
					var index = this.configList.findIndex((conf) => conf.file == file);
					if (index > 0) {
						this.configList.slice(index, index + 1);
						window.DeleteConfig(file);
						window.SaveConfig("configs.json", JSON.stringify(this.configList));
					}
				}
				catch (error) {
					console.log(error);
				}
			},
			saveConfig: function(file) {
				if (!file ||
					!file.length ||
					file.length > 255) {
					this.showSaveSnackbarFail = true;
					return;
				}
				
				try {
					let result = {};
					
					DataKeys.forEach((k) => {
						result[k] = this[k];
					});
					
					let index = this.configList.findIndex((conf) => conf.file == file);
					
					if (index < 0) {
						this.configList.push({
							file,
							date: new Date()
						});
					}
					else {
						this.configList[index].date = new Date();
					}
					
					let json = JSON.stringify(result);
					
					if (window.SaveConfig(file, json)) {
						window.SaveConfig("configs.json", JSON.stringify(this.configList));
						this.showSaveSnackbarSuccess = true;
					}
					else {
						console.log("Failed to persist config with json: " + json);
						this.showSaveSnackbarFail = true;
					}
				}
				catch (error) {
					this.showSaveSnackbarFail = true;
					console.log(error);
				}
				finally {
					this.showSaveDialog = false;
				}
			},
			loadConfig: function(file) {
				if (!file ||
					!file.length ||
					file.length > 255) {
					this.showLoadSnackbarFail = true;
					return;
				}
				
				try {
					let json = window.LoadConfig(file);
					console.log("Loading json: " + json);
					
					if (json) {
						Object.assign(this, JSON.parse(json));
						this.showLoadSnackbarSuccess = true;
						this.configName = file;
					}
					else {
						console.log("Failed to load config with json: " + json);
						this.showLoadSnackbarFail = true;
					}
				}
				catch (error) {
					this.showLoadSnackbarFail = true;
					console.log(error);
				}
				finally {
					this.showLoadDialog = false;
				}
			},
			switchMenuDelayed: function(menu) {
				this.activeMenu = null;
				if (this.currentMenuTimer) {
					clearTimeout(this.currentMenuTimer);
				}
				this.currentMenuTimer = setTimeout(() => {
					this.activeMenu = menu;
				}, 300);
			},
			saveCurve: function() {
				let points = this.drawCurveContext.points;
				if (points.length == 0) {
					return;
				}
				
				const MaxLen = 64;
				let canvas = this.$refs['curve-canvas'];
				
				if (points.length > MaxLen) {
					let chunks = Math.floor(points.length / MaxLen);
					let result = [];
					for (let i = 0; i < MaxLen; ++i) {
						result.push(points.slice(i * chunks, i * chunks + chunks));
					}
					points = result.reduce((arr, val) => {
						return arr.concat(canvas.height - val.reduce((sum, p) => sum + p.y, 0) / val.length);
					}, []);
				}
				else {
					points = points.map((p) => canvas.height - p.y);
				}
				points.unshift(canvas.height / 2);
                points.push(canvas.height / 2);
				
				this.aim.curves.push({
					points,
					avg: canvas.height / 2, 
					usageProbability: 0,
					pixelDistance: [0, 5000],
					overshootFrequency: 0.5,
					overshootFactor: 1.0,
					overshootDegradation: 1.0,
					overshootMeanX: 2.0,
					overshootMeanY: 2.0,
					overshootSigmaX: 0.5,
					overshootSigmaY: 0.5,
					showOvershootDialog: false
				});
				this.drawCurveContext.points = [];
				this.drawCurveOverlay = false;
				this.aim.selectedCurve = this.aim.curves.length - 1;
			},
			setAimkey: function(ev) {
				if (!ev.key) {
					return;
				}
				
				//this['aim']['aimKey'] = ev.key;
				this.aim.aimKey = ev.key.charCodeAt(0);
			},
			removeSelectedCurve: function() {
				if (this.aim.selectedCurve < 0) {
					return;
				}
				this.aim.curves.splice(this.aim.selectedCurve, 1);
				this.aim.selectedCurve = !this.aim.selectedCurve ? 0 : this.aim.selectedCurve - 1;
			},
			drawEnemyMarker: function(canvas, ctx) {
				ctx.strokeStyle = 'red';
				ctx.fillStyle = 'red';
				ctx.lineJoin = 'round';
				ctx.lineCap = 'round';
				ctx.lineWidth = 5;
				ctx.beginPath();
				ctx.arc(canvas.width - 15, canvas.height / 2, 5, 0, Math.PI*2, true);
				ctx.closePath();
				ctx.fill();
				ctx.lineWidth = 1;
				ctx.moveTo(0, canvas.height / 2);
				ctx.lineTo(canvas.width - 15, canvas.height / 2);
				ctx.stroke();
				ctx.lineWidth = 0.5;
				ctx.strokeText("Enemy", canvas.width - 32, canvas.height / 2 + 20);
			},
			resetCurve: function() {
				this.drawCurveContext.points = [];
				let canvas = this.$refs['curve-canvas'];
				let ctx = canvas.getContext("2d");
				ctx.clearRect(0, 0, canvas.width, canvas.height);
				this.drawEnemyMarker(canvas, ctx);
			},
			drawCurveMouseOver: function(ev) {
				if (ev.buttons != 1 && ev.buttons != 3) {
					return false;
				}
				
				if (ev.offsetX < 0 ||
					ev.offsetY < 0) {
					return false;
				}
				
				let points = this.drawCurveContext.points;
				let lastX = points.length > 0 ? points[points.length - 1].x : 0;
				
				if (ev.offsetX < lastX) {
					return true;
				}
				
				if (points.length == 0) {
					
					points.push({x: ev.offsetX, y: ev.offsetY});
				}
				
				points.push({x: ev.offsetX, y: ev.offsetY});
				let canvas = this.$refs['curve-canvas'];
				let ctx = canvas.getContext("2d");
				ctx.clearRect(0, 0, canvas.width, canvas.height);
				ctx.strokeStyle = 'lightgray';
				ctx.fillStyle = 'lightgray';
				var gradient = ctx.createLinearGradient(0, 0, canvas.width, 0);
				gradient.addColorStop("0", "magenta");
				gradient.addColorStop("0.5", "blue");
				gradient.addColorStop("1.0", "red");
				// Fill with gradient
				ctx.strokeStyle = gradient;
				ctx.lineJoin = 'round';
				ctx.lineCap = 'round';
				ctx.lineWidth = 2;
				
				if (points.length < 3) {
					ctx.beginPath();
					ctx.arc(points[0].x, points[0].y, ctx.lineWidth / 2, 0, Math.PI * 2, 1);
					ctx.fill();
					ctx.closePath();
					return true;
				}
				
				ctx.clearRect(0, 0, canvas.width, canvas.height);
				ctx.beginPath();
				ctx.moveTo(points[0].x, points[0].y);
				
				for (let i = 1; i < points.length - 2; i++) {
					ctx.quadraticCurveTo(points[i].x, points[i].y, 
						(points[i].x + points[i + 1].x) / 2, 
						(points[i].y + points[i + 1].y) / 2);
				}
		
				ctx.quadraticCurveTo(
					points[points.length - 2].x,
					points[points.length - 2].y,
					points[points.length - 1].x,
					points[points.length - 1].y
				);
				ctx.stroke();

				ctx.closePath();
				this.drawEnemyMarker(canvas, ctx);
				return true;
			}
		},
		computed: {
			viewModel: {
				get: function() {
					return this.game.thirdPerson ? 1 : 0;
				},
				set: function(value) {
					this.game.thirdPerson = value > 0 ? true : false;
				}
			},
			currentMenu: {
				get: function() {
					return this.activeMenu;
				},
				set: function(value) {
					this.switchMenuDelayed(value);
				}
			}
		},
		data: () => ({
			drawer: true,
			showSaveDialog: false,
			showLoadDialog: false,
			showSaveSnackbarFail: false,
			showLoadSnackbarFail: false,
			showSaveSnackbarSuccess: false,
			showLoadSnackbarSuccess: false,
			configName: '',
			configList: [],
			loadConfigName: '',
			activeMenu: null,
			currentMenuTimer: null,
			aimTabs: 'aim-general',
			espTabs: 'esp-general',
			spottingTabs: 'spotting-general',
			safetyTabs: 'safety-general',
			miscTabs: 'misc-general',
			settingsTabs: 'settings-general',
			screenshotPolicies: [ 'None', 'Ignore', 'Clean', 'Blacken' ],
			showCurveDialog: false,
			drawCurveContext: {
				points: []
			},
			drawCurveOverlay: false,
			aimBones: [
				"HEAD",
				"NECK",
				"BODY",
				"LEGS",
				"ARMS"
			],
			menuItems: {
				home: { title: 'Home', icon: 'mdi-home' },
				esp: { title: 'ESP', icon: 'mdi-eye' },
				aim: { title: 'Aim', icon: 'mdi-crosshairs' },
				misc: { title: 'Misc', icon: 'mdi-cogs' },
				settings: { title: 'Settings', icon: 'mdi-cog' },
				safety: { title: 'Safety', icon: 'mdi-security' },
				spotting: { title: 'Spotting', icon: 'mdi-binoculars' }
			},
			esp: {
				disableTerrainCulling: false,
				chams: true,
				nametags: true,
				indicators: true,
				radar: true,
				outlines: true,
				colors: {
					enemy: '#FF0000FF',
					enemyOccluded: '#800000FF',
					allied: '#000080FF',
					alliedOccluded: '#0000FFFF'
				}
			},
			game: {
				noRecoil: false,
				noSpread: false,
				noBreath: false,
				thirdPerson: false,
				bipodAlwaysSet: false,
				bipodEverywhere: false,
				bipodEveryWeapon: false,
			},
			util: {
				enableTriggerbot: false,
				enableAimbot: false,
				inputType: 1,
				screenshotPolicy: "Clean"
			},
			aim: {
				enableRaycast: false,
				speed: 0,
				silent: false,
				accuracy: 1.0,
				velocity: 0.5,
				selectedCurve: -1,
				aimKey: "p",
				curveSmoothing: 5,
				curveVariation: 0,
				curves: [],
				raycastBones: [ "HEAD" ],
				preferredBone: "BODY",
				targetVehicles: false,
				targetSoldiers: true,
				targetAllies: false,
				targetEnemies: true,
				x3DDistance: [0, 3000],
				x2DDistance: 50,
				weights: {
					previous: 0,
					health: 0,
					distance2d: 0,
					distance3d: 0 
				}
			},
			trigger: {
				preferredBone: "BODY",
				x3DDistance: [0, 3000]
			},
			spotting: {
				autoSpot: false,
				spotBot: false,
				noFOV: false,
				noOccluded: false,
				spotTeam: false,
				noVisibolity: false
			}
		}),
		created () {
			this.$options.components.VSparkline.extendOptions.methods.genPath = function() {
				let genPoints = (minValue, maxValue, values, boundary) => {
					let minX = boundary.minX,
					maxX = boundary.maxX,
					minY = boundary.minY,
					maxY = boundary.maxY;
					let totalValues = values.length;
					let gridX = (maxX - minX) / (totalValues - 1);
					let gridY = (maxY - minY) / (maxValue - minValue || 1);
					return values.map(function (value, index) {
						return {
							x: minX + index * gridX,
							y: maxY - (value - minValue) * gridY,
							value: value
						};
					});
				};
	
				let points = genPoints(0, 600, this.normalizedValues, this.boundary);
				
				return this.$createElement('path', {
					attrs: {
						d: genPath(points, this._radius, this.fill, this.parsedHeight),
						fill: this.fill ? `url(#${this._uid})` : 'none',
						stroke: this.fill ? 'none' : `url(#${this._uid})`,
					},
					ref: 'path',
				})
				
			};
			this.$vuetify.theme.dark = true;
		},
		mounted() {
			this.currentMenu = this.menuItems['aim'];
			
			DataKeys.forEach((v) => {
				let iter = function(obj, key, field) {
					console.log(key + '=' + typeof(obj[field]));
					let isArray = Array.isArray(obj[field]);
					
					if (typeof obj[field] == 'object' && !isArray) {
						for (let k in obj[field]) {
							iter(obj[field], key + '.' + k, k);
						}
					}
					else {
						this.$watch(key, (nv) => {
							if (typeof nv === 'undefined') {
								return;
							}
							console.log(key + '=' + nv);
							try {
								window.SetValue(key, nv);
							}
							catch (error) {
								console.log("Failed to set value with error: " + error);
							}
						}, {deep:true});
					}
				}.bind(this);
					
				for (let k in this[v]) {
					iter(this[v], v + '.' + k, k);
				}
			});
			
			let json;
			
			try {
				let configs = window.LoadConfig(ConfigListFile);
				if (configs) {
					console.log("Config file list: " + configs);
					
					this.configList = JSON.parse(configs);
					if (this.configList && this.configList.length) {
						this.configList.sort((lhs, rhs) => {
							return new Date(rhs.date) - new Date(lhs.date);
						});
						if (this.configList.length > 0) {
							json = window.LoadConfig(this.configName = this.configList[0].file);
						}
					}
				}
				else {
					json = window.LoadConfig();
				}
			}
			catch (error) {
				console.log(error);
			}
			
			console.log("Initial json content: " + json);
			
			if (json) {
				Object.assign(this, JSON.parse(json));
			}
		}
	}
</script>