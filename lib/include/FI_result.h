#ifndef FI_RESULT_H
#define FI_RESULT_H

class FI_result {

    private:
        bool m_result;
        int m_resultType;
        int m_injectionTime;
        int m_core;
        char* m_register;

    public:
        FI_result(bool result, int resultType, int injectionTime, int core, char* reg);
        FI_result();
        ~FI_result();

        bool getResult() const { return m_result; }
        int getResultType() const { return m_resultType; }
        int getInjectionTime() const { return m_injectionTime; }
        int getCore() const { return m_core; }
        const char* getRegister() const { return m_register; }
};

#endif