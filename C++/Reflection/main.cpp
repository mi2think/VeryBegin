#include "Reflection.h"
#include <string>
#include <iostream>
using namespace reflection;
using namespace std;

struct Color : public Object
{
	DECLARE_TYPE(Color);
	union
	{
		struct
		{
			unsigned char b;
			unsigned char g;
			unsigned char r;
			unsigned char a;
		};
		unsigned int value;
	};
	Color() : value(0) {}
	Color(unsigned int val) : value(val) {}
};

DEFINE_TYPE(Color, Object)
{
	DEFINE_FIELD_WITH_TAG(value, FieldTag::Public);
}

class Shape : public Object
{
public:
	DECLARE_TYPE(Shape);
	Shape(int id, Color color)
		: id_(id)
		, color_(color)
	{}
	virtual ~Shape() {}

	void SetId(int id) { id_ = id; }
	int GetId() { return id_; }

	void SetColor(Color c) { color_ = c; }
	Color GetColor() { return color_; }
	
	virtual void Print() { cout << "Shape:" << id_ << " color:" << color_.value << endl; }
protected:
	int id_;
	Color color_;
};

DEFINE_TYPE(Shape, Object)
{
	DEFINE_FIELD_WITH_TAG(id_, FieldTag::Protected);
	DEFINE_FIELD_WITH_TAG(color_, FieldTag::Protected);
	DEFINE_METHOD(SetId);
	DEFINE_METHOD(GetId);
	DEFINE_METHOD(SetColor);
	DEFINE_METHOD(GetColor);
	DEFINE_METHOD(Print);
}

class Rectangle : public Shape
{
public:
	DECLARE_TYPE(Rectangle);
	Rectangle(int id, Color color, int width, int height)
		: Shape(id, color)
		, width_(width)
		, height_(height)
	{}

	void SetWidth(int width) { width_ = width; }
	void SetHeight(int height) { height_ = height; }
	void SetWidthAndHeight(int width, int height) { width_ = width; height_ = height; }

	virtual void Print() 
	{
		cout << "Rectangle:" << id_ << " width:" << width_ << " height:" << height_ << endl;
	}
private:
	int width_;
	int height_;
};
DEFINE_TYPE(Rectangle, Shape)
{
	DEFINE_FIELD(width_);
	DEFINE_FIELD(height_);
	DEFINE_METHOD(SetWidth);
	DEFINE_METHOD(SetHeight);
	DEFINE_METHOD(SetWidthAndHeight);
}

class Circle : public Shape
{
public:
	DECLARE_TYPE(Circle);
	Circle(int id, Color color, int radius)
		: Shape(id, color)
		, radius_(radius)
	{}

	void SetRadius(int radius) { radius_ = radius; }

	virtual void Print()
	{
		cout << "Circle:" << id_ << " radius:" << radius_ << endl;
	}
private:
	int radius_;
};
DEFINE_TYPE(Circle, Shape)
{
	DEFINE_FIELD(radius_);
	DEFINE_METHOD(SetRadius);
}

void TestClass(const char* clsName)
{
	Type* type = TypeRegistry::Instance().GetType(clsName);
	if (type)
	{
		cout << ">>>>>>>>>>" << clsName << endl;
		for (auto& field : type->GetFields())
		{
			cout << "field:" << field.GetName() << " "
				 << field.GetFieldTagAsStr() << " "
				 << field.GetType()->GetName() << endl;
		}

		for (auto& method : type->GetMethods())
		{
			cout << "function:" << method->GetName() << " "
				 << method->GetMethodTagAsStr() << endl;
		}
	}
	else
	{
		cout << "error: no type registered for class - " << clsName << endl;
	}
}

void TestInvoke()
{
	{
		cout << ">>>>>>>>>>Shape" << endl;
		Shape shape(1, Color(0x0f));
		shape.Print();

		Type* shapeType = TypeRegistry::Instance().GetType("Shape");
		shapeType->GetMethod("SetId")->Invoke<void>(&shape, 2);
		shape.Print();

		shapeType->GetMethod("SetColor")->Invoke<void>(&shape, Color(0xff));
		shape.Print();
	}

	{
		cout << ">>>>>>>>>>Rectangle" << endl;
		Type* rcType = TypeRegistry::Instance().GetType("Rectangle");
		Rectangle rc(3, 0xff, 60, 40);
		rc.Print();
		rcType->GetMethod("SetWidthAndHeight")->Invoke<void>(&rc, 50, 50);
		rc.Print();
		rcType->GetMethod("SetId")->Invoke<void>(&rc, 4);
		rcType->GetMethod("Print")->Invoke<void>(&rc);
	}

	{
		cout << ">>>>>>>>>>Circle" << endl;
		Shape* shape = new Circle(5, 0x00, 5);
		shape->Print();
		shape->GetType()->GetMethod("SetRadius")->Invoke<void>(shape, 10);
		shape->GetType()->GetMethod("Print")->Invoke<void>(shape);
		delete shape;
	}
}

void TestIsTypeOf()
{
	Type* shapeType = TypeRegistry::Instance().GetType("Shape");
	Type* rectType = TypeRegistry::Instance().GetType("Rectangle");
	Type* circleType = TypeRegistry::Instance().GetType("Circle");

	Shape* rect = new Rectangle(3, 0xff, 60, 40);
	if (rect->IsTypeOf(shapeType))
		cout << "rect is shape" << endl;
	else
		cout << "rect is not shape" << endl;
	
	Shape* circle = new Circle(5, 0x00, 5);
	if (circle->IsTypeOf(shapeType))
		cout << "circle is shape" << endl;
	else
		cout << "circle is not shape" << endl;

	if (circle->IsTypeOf(rectType))
		cout << "circle is rect" << endl;
	else
		cout << "circle is not rect" << endl;

	delete rect;
	delete circle;
}


class Actor : public Object
{
public:
	DECLARE_TYPE(Actor)
	Actor(int id, string& name)
		: id_(id)
		, name_(name)
	{}

	void SetName(const string name) { name_ = name; }
	void SetId(int id) { id_ = id; }
	void SetNameAndId(string name, int id) { name_ = name; id_ = id; }

	const string& GetName() { return name_; }
	int GetId() { return id_; }

	void Print()
	{
		cout << "Actor:" << name_ << " id:" << id_ << endl;
	}
private:
	int id_;
	string name_;
};

DEFINE_TYPE(Actor, Object)
{
	DEFINE_FIELD(id_);
	DEFINE_FIELD(name_);
	DEFINE_METHOD(SetName);
	DEFINE_METHOD(SetId);
	DEFINE_METHOD(SetNameAndId);
	DEFINE_METHOD(GetName);
	DEFINE_METHOD(GetId);
	DEFINE_METHOD(Print);
}

void TestActor()
{
	Actor actor(1, string("actor1"));
	Type* actorType = TypeRegistry::Instance().GetType("Actor");
	actorType->GetMethod("Print")->Invoke<void>(&actor);
	actorType->GetMethod("SetName")->Invoke<void>(&actor, string("actor2"));
	actorType->GetMethod("SetId")->Invoke<void>(&actor, 2);
	actorType->GetMethod("Print")->Invoke<void>(&actor);
	actorType->GetMethod("SetNameAndId")->Invoke<void>(&actor, string("actor3"), 3);
	actorType->GetMethod("Print")->Invoke<void>(&actor);

	string name = actorType->GetMethod("GetName")->Invoke<const string&>(&actor);
	int id = actorType->GetMethod("GetId")->Invoke<int>(&actor);
	assert(name == string("actor3"));
	assert(id == 3);
}

int main()
{
	Reflection reflection;

	TestClass("Color");
	TestClass("Shape");
	TestClass("Rectangle");
	TestClass("Circle");

	TestInvoke();

	TestIsTypeOf();

	TestActor();

	return 0;
}
