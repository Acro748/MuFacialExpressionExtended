#pragma once

namespace Mus {
	class NiVector4 {
	public:
		float x;
		float y;
		float z;
		float w;

	public:
		NiVector4() {
			x = 0.0f;
			y = 0.0f;
			z = 0.0f;
			w = 1.0f;
		};
		NiVector4 (const float a_x, const float a_y, const float a_z, const float a_w) {
			x = a_x;
			y = a_y;
			z = a_z;
			w = a_w;
		};
		NiVector4 (const float a_x, const float a_y, const float a_z) {
			x = a_x;
			y = a_y;
			z = a_z;
			w = 1.0f;
		};
		NiVector4 (const RE::NiPoint3& a_rhs) {
			x = a_rhs.x;
			y = a_rhs.y;
			z = a_rhs.z;
			w = 1.0f;
		};
		NiVector4 (const DirectX::XMFLOAT4& a_rhs) {
			x = a_rhs.x;
			y = a_rhs.y;
			z = a_rhs.z;
			w = a_rhs.w;
		};
		NiVector4 (const NiVector4& a_rhs) {
			x = a_rhs.x;
			y = a_rhs.y;
			z = a_rhs.z;
			w = a_rhs.w;
		};

		NiVector4 operator*(const NiVector4& a_rhs) const {
			return NiVector4(x * a_rhs.x, y * a_rhs.y, z * a_rhs.z, w);
		};
		NiVector4 operator/(const NiVector4& a_rhs) const {
			return NiVector4(x / a_rhs.x, y / a_rhs.y, z / a_rhs.z, w);
		};
		NiVector4 operator+(const NiVector4& a_rhs) const {
			return NiVector4(x + a_rhs.x, y + a_rhs.y, z + a_rhs.z, w);
		};
		NiVector4 operator-(const NiVector4& a_rhs) const {
			return NiVector4(x - a_rhs.x, y - a_rhs.y, z - a_rhs.z, w);
		};

		NiVector4& operator*=(const NiVector4& a_rhs) {
			x *= a_rhs.x;
			y *= a_rhs.y;
			z *= a_rhs.z;
			return *this;
		};
		NiVector4& operator/=(const NiVector4& a_rhs) {
			x /= a_rhs.x;
			y /= a_rhs.y;
			z /= a_rhs.z;
			return *this;
		};
		NiVector4& operator+=(const NiVector4& a_rhs) {
			x += a_rhs.x;
			y += a_rhs.y;
			z += a_rhs.z;
			return *this;
		};
		NiVector4& operator-=(const NiVector4& a_rhs) {
			x -= a_rhs.x;
			y -= a_rhs.y;
			z -= a_rhs.z;
			return *this;
		};

		NiVector4 operator*(const RE::NiPoint3& a_rhs) const {
			return NiVector4(x * a_rhs.x, y * a_rhs.y, z * a_rhs.z, w);
		};
		NiVector4 operator/(const RE::NiPoint3& a_rhs) const {
			return NiVector4(x / a_rhs.x, y / a_rhs.y, z / a_rhs.z, w);
		};
		NiVector4 operator+(const RE::NiPoint3& a_rhs) const {
			return NiVector4(x + a_rhs.x, y + a_rhs.y, z + a_rhs.z, w);
		};
		NiVector4 operator-(const RE::NiPoint3& a_rhs) const {
			return NiVector4(x - a_rhs.x, y - a_rhs.y, z - a_rhs.z, w);
		};

		NiVector4& operator*=(const RE::NiPoint3& a_rhs) {
			x *= a_rhs.x;
			y *= a_rhs.y;
			z *= a_rhs.z;
			return *this;
		};
		NiVector4& operator/=(const RE::NiPoint3& a_rhs) {
			x /= a_rhs.x;
			y /= a_rhs.y;
			z /= a_rhs.z;
			return *this;
		};
		NiVector4& operator+=(const RE::NiPoint3& a_rhs) {
			x += a_rhs.x;
			y += a_rhs.y;
			z += a_rhs.z;
			return *this;
		};
		NiVector4& operator-=(const RE::NiPoint3& a_rhs) {
			x -= a_rhs.x;
			y -= a_rhs.y;
			z -= a_rhs.z;
			return *this;
		};

		NiVector4 operator*(const float a_scalar) const {
			return NiVector4(x * a_scalar, y * a_scalar, z * a_scalar, w);
		};
		NiVector4 operator/(const float a_scalar) const {
			return NiVector4(x / a_scalar, y / a_scalar, z / a_scalar, w);
		};

		NiVector4& operator*=(const float a_scalar) {
			x *= a_scalar;
			y *= a_scalar;
			z *= a_scalar;
			return *this;
		};
		NiVector4& operator/=(const float a_scalar) {
			x /= a_scalar;
			y /= a_scalar;
			z /= a_scalar;
			return *this;
		};

		bool operator==(const NiVector4& a_rhs) const {
			return (x == a_rhs.x && y == a_rhs.x && z == a_rhs.x);
		};
		bool operator==(const RE::NiPoint3& a_rhs) const {
			return (x == a_rhs.x && y == a_rhs.x && z == a_rhs.x);
		};

		NiVector4& operator=(const RE::NiPoint3& a_rhs) {
			x = a_rhs.x;
			y = a_rhs.y;
			z = a_rhs.z;
			return *this;
		};
		NiVector4& operator=(const DirectX::XMFLOAT4& a_rhs) {
			x = a_rhs.x;
			y = a_rhs.y;
			z = a_rhs.z;
			w = a_rhs.w;
			return *this;
		};

		RE::NiPoint3 AsNiPoint3() const {
			RE::NiPoint3 p3 = RE::NiPoint3(x, y, z);
			return p3;
		};
		DirectX::XMFLOAT4 AsDXVector4() const {
			DirectX::XMFLOAT4 dxv4 = DirectX::XMFLOAT4(x, y, z, w);
			return dxv4;
		};
		DirectX::XMFLOAT4* AsDXVector4Ptr() {
			return reinterpret_cast<DirectX::XMFLOAT4*>(this);
		};
		NiVector4 Copy() {
			NiVector4 nv4 = NiVector4(x, y, z, w);
			return nv4;
		};
	};
}