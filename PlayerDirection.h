#pragma once

// 自機の方向
// 数値に非常に大きな意味を持たせているため、変更禁止!!
// xを10の位、yを1の位に配置し、(0,0)=55とした二次元空間でのベクトル配置
enum Direction {
	//kLeftDown = 44,
	//kLeft = 45,
	//kLeftUp = 46,

	kCenter = 55,		// 中央

	kUp = 56,			// 上
	kRightUp = 66,		// 右上

	kRight = 65,		// 右

	kRightDown = 64,	// 右下
	kDown = 54,			// 下
};