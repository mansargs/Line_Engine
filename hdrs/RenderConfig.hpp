#pragma once

namespace lge {
	class renderConfig {
		private:
			float scaleFactor{1.0f};
			bool  topView {true};
			bool  isometric {false};
			float offsetX{0.0f};
			float offsetY{0.0f};
			float rotateX{0.0f};
			float rotateY{0.0f};
			float rotateZ{0.0f};
		public:
			float getScaleFactor() const;
			float getOffsetX() const;
			float getOffsetY() const;
			float getRotateX() const;
			float getRotateY() const;
			float getRotateZ() const;
			bool  getTopView() const;
			bool  getIsometric() const;
			void setScaleFactor(float scale);
			void setOffsetX(float offset);
			void setOffsetY(float offset);
			void setRotateX(float angle);
			void setRotateY(float angle);
			void setRotateZ(float angle);
			void setTopView(bool top);
			void setIsometric(bool iso);
	};
}
