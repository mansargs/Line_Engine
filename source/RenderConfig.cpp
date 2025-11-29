#include "../hdrs/RenderConfig.hpp"

namespace lge {
	float renderConfig::getScaleFactor() const {
		return scaleFactor;
	}

	float renderConfig::getOffsetX() const {
		return offsetX;
	}

	float renderConfig::getOffsetY() const {
		return offsetY;
	}

	float renderConfig::getRotateX() const {
		return rotateX;
	}

	float renderConfig::getRotateY() const {
		return rotateY;
	}

	float renderConfig::getRotateZ() const {
		return rotateZ;
	}

	bool renderConfig::getTopView() const {
		return topView;
	}

	bool renderConfig::getIsometric() const {
		return isometric;
	}

	bool renderConfig::getDragging() const {
		return dragging;
	}

	int renderConfig::getMouseLastX() const {
		return mouseLastX;
	}
	int renderConfig::getMouseLastY() const {
		return mouseLastY;
	}

	void renderConfig::setScaleFactor(float scale) {
		scaleFactor = scale;
	}

	void renderConfig::setOffsetX(float offset) {
		offsetX = offset;
	}

	void renderConfig::setOffsetY(float offset) {
		offsetY = offset;
	}

	void renderConfig::setRotateX(float angle) {
		rotateX = angle;
	}

	void renderConfig::setRotateY(float angle) {
		rotateY = angle;
	}

	void renderConfig::setRotateZ(float angle) {
		rotateZ = angle;
	}

	void renderConfig::setTopView(bool top) {
		topView = top;
	}

	void renderConfig::setIsometric(bool iso) {
		isometric = iso;
	}

	void renderConfig::setDragging(bool drag) {
		dragging = drag;
	}

	void renderConfig::setMouseLastX(int x) {
		mouseLastX = x;
	}

	void renderConfig::setMouseLastY(int y) {
		mouseLastY = y;
	}

}  // namespace lge
