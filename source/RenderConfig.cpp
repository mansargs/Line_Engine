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
}  // namespace lge
