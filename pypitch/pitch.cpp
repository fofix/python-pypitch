/* This file is copied from /game/pitch.cc in the source code for Performous.
 *
 * Performous is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Performous is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Performous; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

/* stump: make it buildable with MSVC */
#ifndef __GNUC__
#define _USE_MATH_DEFINES
#ifndef NOMINMAX
#define NOMINMAX
#endif
// Visual Studio C++ 9.0 / python 2.7
#if (_MSC_VER == 1500)
#define remainder fmod
#endif
#endif

#include "pitch.hpp"

#include "util.hpp"
#include "fft.hpp"
#include <algorithm>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>

#ifndef M_PI
#endif

// Limit the range to avoid noise and useless computation
static const double FFT_MINFREQ = 45.0;
static const double FFT_MAXFREQ = 5000.0;

Tone::Tone():
	freq(0.0),
	db(-getInf()),
	stabledb(-getInf()),
	age()
{
	for (std::size_t i = 0; i < MAXHARM; ++i)
		harmonics[i] = -getInf();
}

void Tone::print() const {
	if (age < Tone::MINAGE) return;
	std::cout << std::fixed << std::setprecision(1) << freq << " Hz, age " << age << ", " << db << " dB:";
	for (std::size_t i = 0; i < 8; ++i) std::cout << " " << harmonics[i];
	std::cout << std::endl;
}

bool Tone::operator==(double f) const {
	return std::abs(freq / f - 1.0) < 0.05;
}

Analyzer::Analyzer(double rate, std::size_t step):
	m_step(step),
	m_rate(rate),
	m_window(FFT_N),
	m_bufRead(0),
	m_bufWrite(0),
	m_fftLastPhase(FFT_N / 2),
	m_peak(0.0),
	m_oldfreq(0.0)
{
	// Hamming window
	for (size_t i=0; i < FFT_N; i++) {
		m_window[i] = float(0.53836 - 0.46164 * std::cos(2.0 * M_PI * i / (FFT_N - 1)));
	}
}

namespace {
	struct Peak {
		double freq;
		double db;
		bool harm[Tone::MAXHARM];
		Peak(double _freq = 0.0, double _db = -getInf()):
			freq(_freq), db(_db)
		{
			for (std::size_t i = 0; i < Tone::MAXHARM; ++i) harm[i] = false;
		}
		void clear() {
			freq = 0.0;
			db = -getInf();
		}
	};

	Peak& match(std::vector<Peak>& peaks, std::size_t pos) {
		std::size_t best = pos;
		if (peaks[pos - 1].db > peaks[best].db) best = pos - 1;
		if (peaks[pos + 1].db > peaks[best].db) best = pos + 1;
		return peaks[best];
	}
}

static const double FORMANT_START[] = {300, 700, 1800, 3000};
static const double FORMANT_END[] = {1300, 2400, 3300, 4400};
std::vector<double> Analyzer::getFormants() const {
	std::vector<double> formants = std::vector<double>(3, 0.0);
	for (int i = 0; i < 3; i++) {
		double minfreq = FORMANT_START[i];
		double maxfreq = FORMANT_END[i];
		const Tone* maxtone = NULL;
		for (tones_t::const_iterator t = m_tones.begin(); t != m_tones.end(); t++) {
			if (t->freq < minfreq || t->freq > maxfreq || t->age < Tone::MINAGE)
				continue;
			if (i > 0 && t->freq < formants[i-1])
				continue;
			if (maxtone) {
				if (t->db > maxtone->db)
					maxtone = &*t;
			} else
				maxtone = &*t;
		}
		if (maxtone)
			formants[i] = maxtone->freq;
		else
			formants[i] = 0.0;
	}
	return formants;
}

bool Analyzer::calcFFT() {
	float pcm[FFT_N];
	size_t r = m_bufRead;
	if ((BUF_N + m_bufWrite - r) % BUF_N <= FFT_N) return false;
	for (size_t i = 0; i < FFT_N; ++i) {
		pcm[i] = m_buf[(r + i) % BUF_N];
	}
	m_bufRead = (r + m_step) % BUF_N;
	// Calculate FFT
	m_fft = da::fft<FFT_P>(pcm, m_window);
	return true;
}

void Analyzer::calcTones() {
	// Precalculated constants
	const double freqPerBin = m_rate / FFT_N;
	const double phaseStep = 2.0 * M_PI * m_step / FFT_N;
	const double normCoeff = 1.0 / FFT_N;
	const double minMagnitude = pow(10, -100.0 / 20.0) / normCoeff; // -100 dB
	// Limit frequency range of processing
	const size_t kMin = std::max(size_t(1), size_t(FFT_MINFREQ / freqPerBin));
	const size_t kMax = std::min(FFT_N / 2, size_t(FFT_MAXFREQ / freqPerBin));
	std::vector<Peak> peaks(kMax + 1); // One extra to simplify loops
	for (size_t k = 1; k <= kMax; ++k) {
		double magnitude = std::abs(m_fft[k]);
		double phase = std::arg(m_fft[k]);
		// process phase difference
		double delta = phase - m_fftLastPhase[k];
		m_fftLastPhase[k] = float(phase);
		delta -= k * phaseStep;  // subtract expected phase difference
		delta = remainder(delta, 2.0 * M_PI);  // map delta phase into +/- M_PI interval
		delta /= phaseStep;  // calculate diff from bin center frequency
		double freq = (k + delta) * freqPerBin;  // calculate the true frequency
		if (freq > 1.0 && magnitude > minMagnitude) {
			peaks[k].freq = freq;
			peaks[k].db = 20.0 * log10(normCoeff * magnitude);
		}
	}
	// Prefilter peaks
	double prevdb = peaks[0].db;
	for (size_t k = 1; k < kMax; ++k) {
		double db = peaks[k].db;
		if (db > prevdb) peaks[k - 1].clear();
		if (db < prevdb) peaks[k].clear();
		prevdb = db;
	}
	// Find the tones (collections of harmonics) from the array of peaks
	tones_t tones;
	for (size_t k = kMax - 1; k >= kMin; --k) {
		if (peaks[k].db < -70.0) continue;
		// Find the best divider for getting the fundamental from peaks[k]
		std::size_t bestDiv = 1;
		int bestScore = 0;
		for (std::size_t div = 2; div <= Tone::MAXHARM && k / div > 1; ++div) {
			double freq = peaks[k].freq / div; // Fundamental
			int score = 0;
			for (std::size_t n = 1; n < div && n < 8; ++n) {
				Peak& p = match(peaks, k * n / div);
				--score;
				if (p.db < -90.0 || std::abs(p.freq / n / freq - 1.0) > .03) continue;
				if (n == 1) score += 4; // Extra for fundamental
				score += 2;
			}
			if (score > bestScore) {
				bestScore = score;
				bestDiv = div;
			}
		}
		// Construct a Tone by combining the fundamental frequency (freq) and all harmonics
		Tone t;
		std::size_t count = 0;
		double freq = peaks[k].freq / bestDiv;
		t.db = peaks[k].db;
		for (std::size_t n = 1; n <= bestDiv; ++n) {
			// Find the peak for n'th harmonic
			Peak& p = match(peaks, k * n / bestDiv);
			if (std::abs(p.freq / n / freq - 1.0) > .03) continue; // Does it match the fundamental freq?
			if (p.db > t.db - 10.0) {
				t.db = std::max(t.db, p.db);
				++count;
				t.freq += p.freq / n;
			}
			t.harmonics[n - 1] = p.db;
			p.clear();
		}
		t.freq /= count;
		// If the tone seems strong enough, add it (-3 dB compensation for each harmonic)
		if (t.db > -50.0 - 3.0 * count) {
			t.stabledb = t.db;
			tones.push_back(t);
		}
	}
	mergeWithOld(tones);
	m_tones.swap(tones);
}

void Analyzer::mergeWithOld(tones_t& tones) const {
	tones.sort();
	tones_t::iterator it = tones.begin();
	for (tones_t::const_iterator oldit = m_tones.begin(); oldit != m_tones.end(); ++oldit) {
		while (it != tones.end() && *it < *oldit) ++it;
		if (it == tones.end() || *it != *oldit) {
			if (oldit->db > -80.0) {
				Tone& t = *tones.insert(it, *oldit);
				t.db -= 5.0;
				t.stabledb -= 0.1;
			}
		} else if (*it == *oldit) {
			it->age = oldit->age + 1;
			it->stabledb = 0.8 * oldit->stabledb + 0.2 * it->db;
			it->freq = 0.5 * oldit->freq + 0.5 * it->freq;
		}
	}
}

void Analyzer::process() {
	while (calcFFT()) calcTones();
}
