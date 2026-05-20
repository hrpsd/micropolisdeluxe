// ==================================================================================
// = NAS2D
// = Copyright © 2008 - 2020 New Age Software
// ==================================================================================
// = NAS2D is distributed under the terms of the zlib license. You are free to copy,
// = modify and distribute the software under the terms of the zlib license.
// =
// = Acknowledgement of your use of NAS2D is appriciated but is not required.
// ==================================================================================

#pragma once

#include "Point.h"
#include "Vector.h"


template <typename BaseType>
struct MRectangle
{
	BaseType x = 0;
	BaseType y = 0;
	BaseType width = 0;
	BaseType height = 0;

	// Factory method
	constexpr static MRectangle<BaseType> Create(MPoint<BaseType> startPoint, Vector<BaseType> size) {
		return {
			startPoint.x,
			startPoint.y,
			size.x,
			size.y
		};
	}

	// Factory method
	constexpr static MRectangle<BaseType> Create(MPoint<BaseType> startPoint, MPoint<BaseType> endPoint) {
		return {
			startPoint.x,
			startPoint.y,
			endPoint.x - startPoint.x,
			endPoint.y - startPoint.y
		};
	}

	constexpr bool operator==(const MRectangle& rect) const {
		return (x == rect.x) && (y == rect.y) && (width == rect.width) && (height == rect.height);
	}
	constexpr bool operator!=(const MRectangle& rect) const {
		return !(*this == rect);
	}

	constexpr Vector<BaseType> size() const {
		return {width, height};
	}

	constexpr MPoint<BaseType> startPoint() const {
		return {x, y};
	}

	constexpr MPoint<BaseType> endPoint() const {
		return MPoint{x, y} + Vector{width, height};
	}

	constexpr MPoint<BaseType> crossXPoint() const {
		return {x + width, y};
	}

	constexpr MPoint<BaseType> crossYPoint() const {
		return {x, y + height};
	}

	constexpr bool null() const {
		return (width == 0) || (height == 0);
	}

	void size(Vector<BaseType> newSize) {
		width = newSize.x;
		height = newSize.y;
	}

	void startPoint(MPoint<BaseType> newStartPoint) {
		x = newStartPoint.x;
		y = newStartPoint.y;
	}

	constexpr MRectangle inset(BaseType amount) const {
		return {x + amount, y + amount, width - 2 * amount, height - 2 * amount};
	}

	constexpr MRectangle inset(Vector<BaseType> amount) const {
		return {x + amount.x, y + amount.y, width - 2 * amount.x, height - 2 * amount.y};
	}

	constexpr MRectangle inset(Vector<BaseType> amountStart, Vector<BaseType> amountEnd) const {
		return {x + amountStart.x, y + amountStart.y, width - amountStart.x - amountEnd.x, height - amountStart.y - amountEnd.y};
	}

	constexpr MRectangle skewBy(const Vector<BaseType>& scaleFactor) const {
		return Create(startPoint().skewBy(scaleFactor), size().skewBy(scaleFactor));
	}

	constexpr MRectangle skewInverseBy(const Vector<BaseType>& scaleFactor) const {
		return Create(startPoint().skewInverseBy(scaleFactor), size().skewInverseBy(scaleFactor));
	}

	template <typename NewBaseType>
	constexpr operator MRectangle<NewBaseType>() const {
		return {
			static_cast<NewBaseType>(x),
			static_cast<NewBaseType>(y),
			static_cast<NewBaseType>(width),
			static_cast<NewBaseType>(height)
		};
	}

	template <typename NewBaseType>
	constexpr MRectangle<NewBaseType> to() const {
		return static_cast<MRectangle<NewBaseType>>(*this);
	}

	// Start point inclusive (x, y), endpoint exclusive (x + width, y + height)
	// Area in interval notation: [x .. x + width), [y .. y + height)
	constexpr bool contains(const MPoint<BaseType>& point) const {
		auto px = point.x;
		auto py = point.y;
		return ((x <= px) && (px < x + width)) && ((y <= py) && (py < y + height));
	}

	// Start point inclusive (x, y), endpoint exclusive (x + width, y + height)
	// Area in interval notation: [x .. x + width), [y .. y + height)
	constexpr bool overlaps(const MRectangle& rect) const {
		return ((x < rect.x + rect.width) && (rect.x < x + width)) && ((y < rect.y + rect.height) && (rect.y < y + height));
	}

	constexpr MPoint<BaseType> center() const {
		return {x + (width / 2), y + (height / 2)};
	}
};


template <typename BaseType>
MRectangle(BaseType, BaseType, BaseType, BaseType) -> MRectangle<BaseType>;
