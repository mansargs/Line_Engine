#pragma once

namespace lge {
	class renderConfig {
		private:
			bool  dragging {false};
			bool  topView {false};
			bool  isometric {true};
			float scaleFactor{1.0f};
			float scale {1.0f};
			float offsetX{0.0f};
			float offsetY{0.0f};
			float rotateX{35.0f};
			float rotateY{45.0f};
			float rotateZ{0.0f};
			int mouseLastX{0};
			int mouseLastY{0};
		public:
			float getScale() const;
			float getScaleFactor() const;
			float getOffsetX() const;
			float getOffsetY() const;
			float getRotateX() const;
			float getRotateY() const;
			float getRotateZ() const;
			bool  getTopView() const;
			bool  getIsometric() const;
			bool  getDragging() const;
			int   getMouseLastX() const;
			int   getMouseLastY() const;
			void setScale(float scale);
			void setScaleFactor(float scale);
			void setOffsetX(float offset);
			void setOffsetY(float offset);
			void setRotateX(float angle);
			void setRotateY(float angle);
			void setRotateZ(float angle);
			void setTopView(bool top);
			void setIsometric(bool iso);
			void setDragging(bool drag);
			void setMouseLastX(int x);
			void setMouseLastY(int y);
	};
}
