/*
	MIDI-MT is a high level application driver for USB MIDI control surface.
	(c) CC 2023-2024, MIT

	MMTPLUGINx* DLL (Remote Controls Plugin)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!

	Original from:
	*****
	sha1_.hpp is a repackaging of the sha1_.cpp and sha1_.h files from the smallsha1
	library (http://code.google.com/p/smallsha1/) into a single header suitable for
	use as a header only library. This conversion was done by Peter Thorson
	(webmaster@zaphoyd.com) in 2013. All modifications to the code are redistributed
	under the same license as the original, which is listed below.
	*****

	Copyright (c) 2011, Micael Hildenborg
	All rights reserved.

 */

#pragma once

namespace Common {
	namespace WS {

		class AKEY {
		private:
			static constexpr std::string_view base64_chars_ =
				"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
				"abcdefghijklmnopqrstuvwxyz"
				"0123456789+/";

			static inline unsigned int rol_(unsigned int value, unsigned int steps) {
				return ((value << steps) | (value >> (32 - steps)));
			}
			static inline void clear_buffer_(unsigned int* buffer) {
				for (int pos = 16; --pos >= 0;) {
					buffer[pos] = 0;
				}
			}
			static inline void inner_hash_(unsigned int* result, unsigned int* w) {
				unsigned int a = result[0];
				unsigned int b = result[1];
				unsigned int c = result[2];
				unsigned int d = result[3];
				unsigned int e = result[4];

				int round{ 0 };

				#define msha1_(func,val) \
							{ \
								const unsigned int t = rol_(a, 5) + (func) + e + val + w[round]; \
								e = d; \
								d = c; \
								c = rol_(b, 30); \
								b = a; \
								a = t; \
							}

				while (round < 16) {
					msha1_((b & c) | (~b & d), 0x5a827999)
						++round;
				}
				while (round < 20) {
					w[round] = rol_((w[round - 3] ^ w[round - 8] ^ w[round - 14] ^ w[round - 16]), 1);
					msha1_((b & c) | (~b & d), 0x5a827999)
						++round;
				}
				while (round < 40) {
					w[round] = rol_((w[round - 3] ^ w[round - 8] ^ w[round - 14] ^ w[round - 16]), 1);
					msha1_(b ^ c ^ d, 0x6ed9eba1)
						++round;
				}
				while (round < 60) {
					w[round] = rol_((w[round - 3] ^ w[round - 8] ^ w[round - 14] ^ w[round - 16]), 1);
					msha1_((b & c) | (b & d) | (c & d), 0x8f1bbcdc)
						++round;
				}
				while (round < 80) {
					w[round] = rol_((w[round - 3] ^ w[round - 8] ^ w[round - 14] ^ w[round - 16]), 1);
					msha1_(b ^ c ^ d, 0xca62c1d6)
						++round;
				}

				#undef msha1_

				result[0] += a;
				result[1] += b;
				result[2] += c;
				result[3] += d;
				result[4] += e;
			}
			static inline void sha1_(void const* src, size_t blength, unsigned char* hash) {
				unsigned int result[5] = { 0x67452301, 0xefcdab89, 0x98badcfe,
										   0x10325476, 0xc3d2e1f0 };

				unsigned char const* sarray = (unsigned char const*)src;
				unsigned int w[80]{};
				size_t eblock{ 0 }, cblock{ 0 };

				if (blength >= 64) {
					size_t const eoblocks = blength - 64;

					while (cblock <= eoblocks) {
						eblock = cblock + 64;

						for (int pos = 0; cblock < eblock; cblock += 4) {
							w[pos++] = (unsigned int)sarray[cblock + 3]
								| (((unsigned int)sarray[cblock + 2]) << 8)
								| (((unsigned int)sarray[cblock + 1]) << 16)
								| (((unsigned int)sarray[cblock]) << 24);
						}
						inner_hash_(result, w);
					}
				}

				eblock = blength - cblock;
				clear_buffer_(w);
				size_t lblockb{ 0 };
				for (; lblockb < eblock; ++lblockb) {
					w[lblockb >> 2] |= (unsigned int)sarray[lblockb + cblock] << ((3 - (lblockb & 3)) << 3);
				}

				w[lblockb >> 2] |= 0x80 << ((3 - (lblockb & 3)) << 3);
				if (eblock >= 56) {
					inner_hash_(result, w);
					clear_buffer_(w);
				}
				w[15] = static_cast<unsigned int>(blength << 3);
				inner_hash_(result, w);

				for (int hashByte = 20; --hashByte >= 0;) {
					hash[hashByte] = (result[hashByte >> 2] >> (((3 - hashByte) & 0x3) << 3)) & 0xff;
				}
			}

			template <typename T>
			static inline std::string b64_encode_(const T *input, size_t len) {
				std::string ret{};
				int i = 0;
				int j = 0;
				T ca_3[3]{ 0 };
				T ca_4[4]{ 0 };

				while (len--) {
					ca_3[i++] = *(input++);
					if (i == 3) {
						ca_4[0] = (ca_3[0] & 0xfc) >> 2;
						ca_4[1] = ((ca_3[0] & 0x03) << 4) +
							((ca_3[1] & 0xf0) >> 4);
						ca_4[2] = ((ca_3[1] & 0x0f) << 2) +
							((ca_3[2] & 0xc0) >> 6);
						ca_4[3] = ca_3[2] & 0x3f;

						for (i = 0; (i < 4); i++) {
							ret += base64_chars_[ca_4[i]];
						}
						i = 0;
					}
				}

				if (i) {
					for (j = i; j < 3; j++) {
						ca_3[j] = '\0';
					}

					ca_4[0] = (ca_3[0] & 0xfc) >> 2;
					ca_4[1] = ((ca_3[0] & 0x03) << 4) +
						((ca_3[1] & 0xf0) >> 4);
					ca_4[2] = ((ca_3[1] & 0x0f) << 2) +
						((ca_3[2] & 0xc0) >> 6);
					ca_4[3] = ca_3[2] & 0x3f;

					for (j = 0; (j < i + 1); j++) {
						ret += base64_chars_[ca_4[j]];
					}

					while ((i++ < 3)) {
						ret += '=';
					}
				}
				return ret;
			}

		public:

			static const std::string sec_websocket(const std::string& key) {
				unsigned char m[20]{};
				const std::string key_ = key + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
				sha1_(key_.c_str(), key_.length(), m);
				return b64_encode_(m, std::size(m));
			}

			template <typename T>
			static const std::string b64_encode(const T* v, size_t sz) {
				return b64_encode_<T>(v, sz);
			}
			template <typename T>
			static const std::string b64_encode(const T& s) {
				return b64_encode_(s.data(), s.length());
			}

			static const std::wstring path_escaped(const std::wstring& s) {
				try {
					std::wstring w(s);
					size_t p{ 0 };
					while ((p = w.find(L'\\', p)) != std::wstring::npos) {
						w.replace(p, 1, L"\\\\");
						p += 3;
						if (p >= w.length()) break;
					}
					return w;
				} catch (...) {}
				return std::wstring();
			}
		};
	}
}