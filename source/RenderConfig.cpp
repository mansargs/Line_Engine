#include "../hdrs/RenderConfig.hpp"

namespace lge {
	float RenderConfig::getScaleFactor() const {
		return scaleFactor;
	}

	float RenderConfig::getOffsetX() const {
		return offsetX;
	}

	float RenderConfig::getOffsetY() const {
		return offsetY;
	}

	float RenderConfig::getRotateX() const {
		return rotateX;
	}

	float RenderConfig::getRotateY() const {
		return rotateY;
	}

	float RenderConfig::getRotateZ() const {
		return rotateZ;
	}

	bool RenderConfig::getTopView() const {
		return topView;
	}

	bool RenderConfig::getIsometric() const {
		return isometric;
	}

	bool RenderConfig::getDragging() const {
		return dragging;
	}

	int RenderConfig::getMouseLastX() const {
		return mouseLastX;
	}
	int RenderConfig::getMouseLastY() const {
		return mouseLastY;
	}

	void RenderConfig::setScaleFactor(float scale) {
		scaleFactor = scale;
	}

	void RenderConfig::setOffsetX(float offset) {
		offsetX = offset;
	}

	void RenderConfig::setOffsetY(float offset) {
		offsetY = offset;
	}

	void RenderConfig::setRotateX(float angle) {
		rotateX = angle;
	}

	void RenderConfig::setRotateY(float angle) {
		rotateY = angle;
	}

	void RenderConfig::setRotateZ(float angle) {
		rotateZ = angle;
	}

	void RenderConfig::setTopView(bool top) {
		topView = top;
	}

	void RenderConfig::setIsometric(bool iso) {
		isometric = iso;
	}

	void RenderConfig::setDragging(bool drag) {
		dragging = drag;
	}

	void RenderConfig::setMouseLastX(int x) {
		mouseLastX = x;
	}

	void RenderConfig::setMouseLastY(int y) {
		mouseLastY = y;
	}

	void RenderConfig::setScale(float scaleVal) {
		scale = scaleVal;
	}

	float RenderConfig::getScale() const {
		return scale;
	}

	float RenderConfig::getZScale() const {
		return zScale;
	}

	void RenderConfig::setZScale(float scale) {
		zScale = scale;
	}

	float RenderConfig::getLineWidth() const {
		return lineWidth;
	}

	void RenderConfig::setLineWidth(float width) {
		lineWidth = width;
	}
}  // namespace lge
