/*
 * Copyright (C) 2020 Chris Holland <zrenfire@gmail.com>
 * Copyright (C) 2012 Martin Gräßlin <mgraesslin@kde.org>
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the
 *GNU General Public License as published by the Free Software Foundation, either version 2 of the
 *License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 *even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program. If
 *not, see <http://www.gnu.org/licenses/>.
 */

// own
#include "ConfigurationModule.h"
#include "InternalSettings.h"
#include "Material.h"

// KF
#include <KCModule>
#include <KColorButton>
#include <KConfigSkeleton>
#include <KCoreConfigSkeleton>
#include <KLocalizedString>
#include <KSharedConfig>

// KDecoration
#include <KDecoration2/DecorationButton>

// Qt
#include <QDebug>

// QWidget
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QLabel>
#include <QRadioButton>
#include <QSpinBox>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>

namespace Material
{

ConfigurationModule::ConfigurationModule(QWidget *parent, const QVariantList &args)
	: KCModule(parent, args)
	, m_titleAlignment(InternalSettings::AlignCenterFullWidth)
	, m_buttonSize(InternalSettings::ButtonDefault)
	, m_shadowSize(InternalSettings::ShadowVeryLarge)
{
	init();
}

void ConfigurationModule::init()
{
	KCoreConfigSkeleton *skel =
		new KCoreConfigSkeleton(KSharedConfig::openConfig(s_configFilename), this);
	skel->setCurrentGroup(QStringLiteral("Windeco"));

	// See fr.po for the messages we can reuse from breeze:
	// https://websvn.kde.org/*checkout*/trunk/l10n-kf5/fr/messages/breeze/breeze_kwin_deco.po

	//--- Tabs
	QTabWidget *tabWidget = new QTabWidget(this);
	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	mainLayout->addWidget(tabWidget);
	mainLayout->addStretch(1);
	setLayout(mainLayout);

	//--- General
	QWidget *generalTab = new QWidget(tabWidget);
	tabWidget->addTab(generalTab, i18nd("breeze_kwin_deco", "General"));
	QFormLayout *generalForm = new QFormLayout(generalTab);
	generalTab->setLayout(generalForm);

	QComboBox *titleAlignment = new QComboBox(generalTab);
	titleAlignment->addItem(i18nd("breeze_kwin_deco", "Left"));
	titleAlignment->addItem(i18nd("breeze_kwin_deco", "Center"));
	titleAlignment->addItem(i18nd("breeze_kwin_deco", "Center (Full Width)"));
	titleAlignment->addItem(i18nd("breeze_kwin_deco", "Right"));
	titleAlignment->addItem(i18n("Hidden"));
	titleAlignment->setObjectName(QStringLiteral("kcfg_TitleAlignment"));
	generalForm->addRow(i18nd("breeze_kwin_deco", "Tit&le alignment:"), titleAlignment);

	QComboBox *buttonSizes = new QComboBox(generalTab);
	buttonSizes->addItem(i18nd("breeze_kwin_deco", "Tiny"));
	buttonSizes->addItem(i18ndc("breeze_kwin_deco", "@item:inlistbox Button size:", "Small"));
	buttonSizes->addItem(i18ndc("breeze_kwin_deco", "@item:inlistbox Button size:", "Medium"));
	buttonSizes->addItem(i18ndc("breeze_kwin_deco", "@item:inlistbox Button size:", "Large"));
	buttonSizes->addItem(i18ndc("breeze_kwin_deco", "@item:inlistbox Button size:", "Very Large"));
	buttonSizes->setObjectName(QStringLiteral("kcfg_ButtonSize"));
	generalForm->addRow(i18nd("breeze_kwin_deco", "B&utton size:"), buttonSizes);

	QDoubleSpinBox *activeOpacity = new QDoubleSpinBox(generalTab);
	activeOpacity->setMinimum(0.0);
	activeOpacity->setMaximum(1.0);
	activeOpacity->setSingleStep(0.05);
	activeOpacity->setObjectName(QStringLiteral("kcfg_ActiveOpacity"));
	generalForm->addRow(i18n("Active opacity:"), activeOpacity);

	QDoubleSpinBox *activeLightness = new QDoubleSpinBox(generalTab);
	activeLightness->setMinimum(0.0);
	activeLightness->setMaximum(2.0);
	activeLightness->setSingleStep(0.01);
	activeLightness->setObjectName(QStringLiteral("kcfg_ActiveLightness"));
	generalForm->addRow(i18n("Active lightness:"), activeLightness);

	QDoubleSpinBox *inactiveOpacity = new QDoubleSpinBox(generalTab);
	inactiveOpacity->setMinimum(0.0);
	inactiveOpacity->setMaximum(1.0);
	inactiveOpacity->setSingleStep(0.05);
	inactiveOpacity->setObjectName(QStringLiteral("kcfg_InactiveOpacity"));
	generalForm->addRow(i18n("Inactive opacity:"), inactiveOpacity);

	QDoubleSpinBox *inactiveLightness = new QDoubleSpinBox(generalTab);
	inactiveLightness->setMinimum(0.0);
	inactiveLightness->setMaximum(2.0);
	inactiveLightness->setSingleStep(0.01);
	inactiveLightness->setObjectName(QStringLiteral("kcfg_InactiveLightness"));
	generalForm->addRow(i18n("Inactive lightness:"), inactiveLightness);

	//--- Menu
	QWidget *menuTab = new QWidget(tabWidget);
	tabWidget->addTab(menuTab, i18n("Menu"));
	QFormLayout *menuForm = new QFormLayout(menuTab);
	menuTab->setLayout(menuForm);

	QLabel *menuLabel = new QLabel(menuTab);
	menuLabel->setText(
		i18n("To enable the Locally Integrated Menus in the titlebar:\nSystem Settings > Window "
			 "Decorations > Titlebar Buttons Tab\nDrag the 'Application Menu' button to bar."));
	menuForm->addRow(QLatin1String(), menuLabel);

	QRadioButton *menuAlwaysShow = new QRadioButton(menuTab);
	menuAlwaysShow->setText(i18n("Always Show Menu"));
	menuAlwaysShow->setObjectName(QStringLiteral("kcfg_MenuAlwaysShow"));
	menuForm->addRow(QLatin1String(), menuAlwaysShow);

	// Since there's no easy way to bind this to !MenuAlwaysShow, we
	// workaround this by marking the button as checked on init.
	// When the config is loaded:
	// * If menuAlwaysShow is toggled true, this will be toggled false.
	// * If menuAlwaysShow is left false, then this remains true.
	QRadioButton *menuRevealOnHover = new QRadioButton(menuTab);
	menuRevealOnHover->setText(i18n("Reveal Menu on Hover"));
	menuRevealOnHover->setChecked(true);
	menuForm->addRow(QLatin1String(), menuRevealOnHover);

	QButtonGroup *menuAlwaysShowGroup = new QButtonGroup(menuTab);
	menuAlwaysShowGroup->addButton(menuAlwaysShow);
	menuAlwaysShowGroup->addButton(menuRevealOnHover);

	//--- Animations
	QWidget *animationsTab = new QWidget(tabWidget);
	tabWidget->addTab(animationsTab, i18nd("breeze_kwin_deco", "Animations"));
	QFormLayout *animationsForm = new QFormLayout(animationsTab);
	animationsTab->setLayout(animationsForm);

	QCheckBox *animationsEnabled = new QCheckBox(animationsTab);
	animationsEnabled->setText(i18nd("breeze_kwin_deco", "Enable animations"));
	animationsEnabled->setObjectName(QStringLiteral("kcfg_AnimationsEnabled"));
	animationsForm->addRow(QLatin1String(), animationsEnabled);

	QSpinBox *animationsDuration = new QSpinBox(animationsTab);
	animationsDuration->setMinimum(0);
	animationsDuration->setMaximum(INT_MAX);
	animationsDuration->setSuffix(i18nd("breeze_kwin_deco", " ms"));
	animationsDuration->setObjectName(QStringLiteral("kcfg_AnimationsDuration"));
	animationsForm->addRow(i18nd("breeze_kwin_deco", "Animations:"), animationsDuration);

	//--- Shadows
	QWidget *shadowTab = new QWidget(tabWidget);
	tabWidget->addTab(shadowTab, i18nd("breeze_kwin_deco", "Shadows"));
	QFormLayout *shadowForm = new QFormLayout(shadowTab);
	shadowTab->setLayout(shadowForm);

	QComboBox *shadowSizes = new QComboBox(shadowTab);
	shadowSizes->addItem(i18ndc("breeze_kwin_deco", "@item:inlistbox Button size:", "None"));
	shadowSizes->addItem(i18ndc("breeze_kwin_deco", "@item:inlistbox Button size:", "Small"));
	shadowSizes->addItem(i18ndc("breeze_kwin_deco", "@item:inlistbox Button size:", "Medium"));
	shadowSizes->addItem(i18ndc("breeze_kwin_deco", "@item:inlistbox Button size:", "Large"));
	shadowSizes->addItem(i18ndc("breeze_kwin_deco", "@item:inlistbox Button size:", "Very Large"));
	shadowSizes->setObjectName(QStringLiteral("kcfg_ShadowSize"));
	shadowForm->addRow(i18nd("breeze_kwin_deco", "Si&ze:"), shadowSizes);

	QSpinBox *shadowStrength = new QSpinBox(shadowTab);
	shadowStrength->setMinimum(25);
	shadowStrength->setMaximum(255);
	// shadowStrength->setSuffix(i18nd("breeze_kwin_deco", "%"));
	shadowStrength->setObjectName(QStringLiteral("kcfg_ShadowStrength"));
	shadowForm->addRow(i18ndc("breeze_kwin_deco",
							  "strength of the shadow (from transparent to opaque)", "S&trength:"),
					   shadowStrength);

	KColorButton *shadowColor = new KColorButton(shadowTab);
	shadowColor->setObjectName(QStringLiteral("kcfg_ShadowColor"));
	shadowForm->addRow(i18nd("breeze_kwin_deco", "Color:"), shadowColor);

	//--- Build Info
	QWidget *aboutTab = new QWidget(tabWidget);
	tabWidget->addTab(aboutTab, i18nd("breeze_kwin_deco", "About"));
	QFormLayout *aboutForm = new QFormLayout(aboutTab);
	aboutTab->setLayout(aboutForm);

	QLabel *aboutVersion = new QLabel(aboutTab);
	aboutVersion->setText(QLatin1String("v1.0"));
	aboutForm->addRow(i18n("Version"), aboutVersion);

	//--- Config Bindings
	skel->addItemInt(QStringLiteral("TitleAlignment"), m_titleAlignment,
					 InternalSettings::AlignCenterFullWidth, QStringLiteral("TitleAlignment"));
	skel->addItemInt(QStringLiteral("ButtonSize"), m_buttonSize, InternalSettings::ButtonDefault,
					 QStringLiteral("ButtonSize"));
	skel->addItemDouble(QStringLiteral("ActiveOpacity"), m_activeOpacity, 1.0,
						QStringLiteral("ActiveOpacity"));
	skel->addItemDouble(QStringLiteral("ActiveLightness"), m_activeLightness, 0.97,
						QStringLiteral("ActiveLightness"));
	skel->addItemDouble(QStringLiteral("InactiveOpacity"), m_inactiveOpacity, 1.0,
						QStringLiteral("InactiveOpacity"));
	skel->addItemDouble(QStringLiteral("InactiveLightness"), m_inactiveLightness, 0.97,
						QStringLiteral("InactiveLightness"));
	skel->addItemBool(QStringLiteral("MenuAlwaysShow"), m_menuAlwaysShow, true,
					  QStringLiteral("MenuAlwaysShow"));
	skel->addItemBool(QStringLiteral("AnimationsEnabled"), m_animationsEnabled, true,
					  QStringLiteral("AnimationsEnabled"));
	skel->addItemInt(QStringLiteral("AnimationsDuration"), m_animationsDuration, 150,
					 QStringLiteral("AnimationsDuration"));
	skel->addItemInt(QStringLiteral("ShadowSize"), m_shadowSize, InternalSettings::ShadowMedium,
					 QStringLiteral("ShadowSize"));
	skel->addItemInt(QStringLiteral("ShadowStrength"), m_shadowStrength, 255,
					 QStringLiteral("ShadowStrength"));
	skel->addItem(new KConfigSkeleton::ItemColor(skel->currentGroup(),
												 QStringLiteral("ShadowColor"), m_shadowColor,
												 QColor(33, 33, 33)),
				  QStringLiteral("ShadowColor"));

	//---
	addConfig(skel, this);
}

} // namespace Material
