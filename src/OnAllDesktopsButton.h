/*
 * Copyright (C) 2020 Chris Holland <zrenfire@gmail.com>
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

#pragma once

// own
#include "Button.h"
#include "Material.h"

// KDecoration
#include <KDecoration2/DecoratedClient>

// Qt
#include <QPainter>

namespace Material
{

class OnAllDesktopsButton
{

public:
	static void init(Button *button, KDecoration2::DecoratedClient *decoratedClient)
	{
		Q_UNUSED(decoratedClient)

		button->setVisible(true);
	}
	static void paintIcon(Button *button, QPainter *painter, const QRectF &iconRect,
						  const qreal gridUnit)
	{
		Q_UNUSED(gridUnit)
		painter->setRenderHints(QPainter::Antialiasing, true);
		button->setPenWidth(painter, gridUnit, 1.25);

		int radius = qMin(iconRect.width(), iconRect.height()) / 2;
		QPoint center(iconRect.center().toPoint());
		painter->drawPolygon(
			QVector<QPointF>{center + QPoint(-radius, 0), center + QPoint(0, -radius),
							 center + QPoint(radius, 0), center + QPoint(0, radius)});
	}
};

} // namespace Material
