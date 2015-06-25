class Entity;
/****************************************************************/
class EntityCallbackBase
{
public:
    virtual void invoke(Entity* e) = 0;
  // add other methods here

};


/****************************************************************/
template<class C>
class EntityCallback : public EntityCallbackBase
{
public:
	typedef void (C::*Invoke_t)(Entity* e);

	EntityCallback(C*c, Invoke_t i)      { _class = c; _invoke = i; }
    EntityCallback()      { _class = 0; _invoke = 0; }
	void setClass(C* c) {_class = c;}
	void setInvoke(Invoke_t i) {_invoke = i;}


protected:

    void invoke(Entity* e)
    {
        if(_class && _invoke) 
        {
            (_class->*_invoke) (e);
        }
    }

private:
    C*           _class;
    Invoke_t	_invoke;
};