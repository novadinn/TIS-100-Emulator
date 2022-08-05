struct TIS {
    TIS();
    ~TIS();
private:
    enum State {
	EDIT,
	RUN
    };
    
    void eventLoop();

    State state_;
};
