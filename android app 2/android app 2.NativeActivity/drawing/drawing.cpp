
namespace drawing
{
	struct viewportgl
	{
		int x, y, width, height;
	};

	void setup_view(int width, int height)
	{
		glViewport(0, 0, width, height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrthof(0, width, 0, height, 1, -1);
	}

	void trinagle(GLfloat x, GLfloat y, GLfloat size, GLfloat angle, color color)
	{
		glColor4f(color.r, color.g, color.b, color.a);

		viewportgl vp{};
		glGetIntegerv(GL_VIEWPORT, (GLint*)&vp);
		y = abs(y - vp.height);
		y -= size;

		GLfloat vertices[] =
		{
			(GLfloat)(x), (GLfloat)(y),
			(GLfloat)(x + size), (GLfloat)(y + size),
			(GLfloat)(x), (GLfloat)(y + size),
		};

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, vertices);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glDisableClientState(GL_VERTEX_ARRAY);
	}

	void box(GLfloat x, GLfloat y, GLfloat size, color color)
	{
		glColor4f(color.r, color.g, color.b, color.a);

		viewportgl vp{};
		glGetIntegerv(GL_VIEWPORT, (GLint*)&vp);
		y = abs(y - vp.height);
		y -= size;

		GLfloat vertices[] =
		{
			(GLfloat)(x), (GLfloat)(y),
			(GLfloat)(x + size), (GLfloat)(y + size),
			(GLfloat)(x), (GLfloat)(y + size),

			(GLfloat)(x), (GLfloat)(y),
			(GLfloat)(x + size), (GLfloat)(y),
			(GLfloat)(x + size), (GLfloat)(y + size),
		};

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, vertices);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawArrays(GL_TRIANGLES, 3, 3);
		glDisableClientState(GL_VERTEX_ARRAY);
	}

	void rectangle(int x, int y, int x1, int x2)
	{

	}
}