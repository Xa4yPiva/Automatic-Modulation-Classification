#define PI  3.14159265
#include <amc.h>
#include <complex>

std::vector<std::complex<double> > AMC::fft(
        std::vector<std::complex<double> > &x)
{
    size_t N = x.size();

    std::vector<std::complex<double> > X(x.size());
    fftw_plan plan = fftw_plan_dft_1d(N,
            reinterpret_cast<fftw_complex*>(&x[0]),
            reinterpret_cast<fftw_complex*>(&X[0]),
			FFTW_FORWARD,
		   	FFTW_ESTIMATE);
    fftw_execute(plan);
    for (size_t n = 0; n< N; ++n)
    {
        X[n] = X[n]/(double)N;
    }
	return X;
}

std::vector<std::complex<double> > AMC::ifft(
        std::vector<std::complex<double> > &X)
{
    size_t N = X.size();
    std::vector<std::complex<double> > x(N);
    fftw_plan plan = fftw_plan_dft_1d(N,
            reinterpret_cast<fftw_complex*>(&X[0]),
            reinterpret_cast<fftw_complex*>(&x[0]),
            FFTW_BACKWARD,
			FFTW_ESTIMATE);
	fftw_execute(plan);
    return x;
}

std::vector<std::complex<double> > AMC::instantaneousSignal(
        std::vector<std::complex<double> > &x)
{
    size_t N = x.size();
	if (N%1 != 1)
	{
		// TODO throw exception
	}
    std::vector<std::complex<double> > X(N);
    X = AMC::fft(x);
    for (size_t n = N/2; n < N; ++n)
	{
		X[n] = std::complex<double>(0,0);
	}
    return ifft(X);
}

std::vector<double> AMC::instantaneousAmplitude(
        std::vector<std::complex<double> > &x)
{
    size_t N = x.size();
	std::vector<std::complex<double> > x_i(N);
    x_i = AMC::instantaneousSignal(x);

    std::vector<double> x_i_abs(N);
    for (size_t n = 0; n < N; ++n)
	{
        x_i_abs[n] = std::abs(x_i[n]);
	}
	return x_i_abs;
}

std::vector<double> AMC::instantaneousPhase(
        std::vector<std::complex<double> > &x)
{
    size_t N = x.size();
	std::vector<std::complex<double> > x_i(N);
    x_i = AMC::instantaneousSignal(x);

    std::vector<double> x_i_phase(N);
    for (size_t n = 0; n < N; ++n)
	{
        x_i_phase[n] = std::arg(x_i[n]);
	}
	return x_i_phase;
}


static std::vector<double> AMC::unwrapPhase(
        std::vector<double> x_i_phase)
{
    size_t N = x_i_phase.size();
    for (size_t n0 = 0; n0 < N-1; ++n0)
	{
        if (x_i_phase[n0] >= x_i_phase[n0+1] + PI)
		{
            for (size_t n1 = n0 + 1; n1 < N; ++n1)
            {
                x_i_phase[n1] += 2*PI;
            }
        }
        if (x_i_phase[n0] < x_i_phase[n0+1] - PI)
        {
            for (size_t n1 = n0 + 1; n1 < N; ++n1)
            {
                x_i_phase[n1] -= 2*PI;
            }
        }
    }

	return x_i_phase;
}

std::vector<double> AMC::unwrappedInstantaneousPhase(
        std::vector<std::complex<double> > &x)
{
    size_t N = x.size();
    std::vector<double> x_i_phase(N);
    x_i_phase = AMC::instantaneousPhase(x);

    return AMC::unwrapPhase(x_i_phase);
}

std::vector<double> AMC::nonLinearUnwrappedInstantaneousPhase(std::vector<std::complex<double> > &x,
        const double &fc,
        const double &fs)
{
    size_t N = x.size();
    std::vector<double> x_i_unwr_phase = AMC::unwrappedInstantaneousPhase(x);
    for (size_t n = 0; n < N; ++n)
    {
        x_i_unwr_phase[n] -= 2*PI*fc*n/fs;
    }

    return x_i_unwr_phase;
}

std::vector<double> AMC::abs(std::vector<double> x)
{
    for (double &xi: x)
    {
        if (xi < 0)
        {
            xi = -xi;
        }
    }
    return x;
}

std::vector<double> AMC::abs(std::vector<std::complex<double> > x)
{
    std::vector<double> a(x.size());
    for (size_t n = 0; n < x.size(); ++n)
    {
        a[n] = std::abs(x[n]);
    }
    return a;
}

double AMC::mean(const std::vector<double> &x)
{
    double sum = 0;
    for(double a:x)
    {
        sum += a;
    }

    return sum/x.size();
}

double AMC::stdDev(const std::vector<double> &x)
{
    double m = AMC::mean(x);
    double sqSum = 0;
    for(double xi:x)
    {
        sqSum += (xi - m)*(xi - m);
    }

    return sqrt(sqSum/x.size());
}

double AMC::sigmaDP(
        std::vector<std::complex<double> > &x,
        const double &fc,
        const double &fs)
{
    size_t N = x.size();
    std::vector<double> a(N);
    a = AMC::nonLinearUnwrappedInstantaneousPhase(x,fc,fs);
    return stdDev(a);
}

double AMC::sigmaAP(
        std::vector<std::complex<double> > &x,
        const double &fc,
        const double &fs)
{
    size_t N = x.size();
    std::vector<double> a(N);
    a = AMC::abs(AMC::nonLinearUnwrappedInstantaneousPhase(x,fc,fs));
    return stdDev(a);
}
