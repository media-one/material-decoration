/*
 * Copyright (C) 2020 Chris Holland <zrenfire@gmail.com>
 * Copyright (C) 2018 Vlad Zagorodniy <vladzzag@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

// own
#include "AppMenuModel.h"
#include "AppMenuButtonGroup.h"

// KDecoration
#include <KDecoration2/Decoration>
#include <KDecoration2/DecorationButton>
#include <KDecoration2/DecorationButtonGroup>

// Qt
#include <QVariant>

namespace Material
{

class AppIconButton;
class ApplicationMenuButton;
class KeepAboveButton;
class CloseButton;
class MaximizeButton;
class MinimizeButton;

class Decoration : public KDecoration2::Decoration
{
    Q_OBJECT

public:
    Decoration(QObject *parent = nullptr, const QVariantList &args = QVariantList());
    ~Decoration() override;

    void paint(QPainter *painter, const QRect &repaintRegion) override;

    static KDecoration2::DecorationButton *createButton(KDecoration2::DecorationButtonType type, KDecoration2::Decoration *decoration, QObject *parent = nullptr);

public slots:
    void init() override;

private:
    void updateBorders();
    void updateResizeBorders();
    void updateTitleBar();
    void updateButtonsGeometry();
    void updateShadow();

    int titleBarHeight() const;
    int captionMinWidth() const;
    int getTextWidth(const QString text) const;
    QPoint windowPos() const;

    QColor titleBarBackgroundColor() const;
    QColor titleBarForegroundColor() const;

    void paintFrameBackground(QPainter *painter, const QRect &repaintRegion) const;
    void paintTitleBarBackground(QPainter *painter, const QRect &repaintRegion) const;
    void paintCaption(QPainter *painter, const QRect &repaintRegion) const;
    void paintButtons(QPainter *painter, const QRect &repaintRegion) const;

    KDecoration2::DecorationButtonGroup *m_leftButtons;
    KDecoration2::DecorationButtonGroup *m_rightButtons;
    AppMenuButtonGroup *m_menuButtons;

    friend class AppMenuButtonGroup;
    friend class CommonButton;
    friend class CommonToggleButton;
    friend class AppIconButton;
    friend class ApplicationMenuButton;
    friend class OnAllDesktopsButton;
    friend class KeepAboveButton;
    friend class KeepBelowButton;
    friend class CloseButton;
    friend class MaximizeButton;
    friend class MinimizeButton;
    friend class TextButton;
    friend class MenuOverflowButton;
};

} // namespace Material
