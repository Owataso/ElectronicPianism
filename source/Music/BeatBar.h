#pragma once

/********************************************/
//	ドゥンドゥンやってるあれ
/********************************************/
class BeatBar
{
public:
	//------------------------------------------------------
	//	コンストラクタ
	//------------------------------------------------------
	BeatBar() :
		m_pImage(new tdn2DObj("DATA/UI/Playing/judge_line.png")), 
		m_tagBeat(Beat()){}

	//------------------------------------------------------
	//	更新・描画
	//------------------------------------------------------
	void Update(int iPlayTime){ m_tagBeat.SetPlayTime(iPlayTime); }
	void Render(int x, int y)
	{
		const float percent(m_tagBeat.GetPercent());
		m_pImage->SetAlpha(Math::Blend(percent, .25f, 1));
		m_pImage->Render(x, y, 1120, 48, 0, 32, 1120, 48);
	}

	//------------------------------------------------------
	//	BPM変更時に呼び出す
	//------------------------------------------------------
	void SetBPM(float fBPM){ m_tagBeat.SetBPM(fBPM); }

	//------------------------------------------------------
	//	初期化
	//------------------------------------------------------
	void Reset(){ m_tagBeat.Reset(); }

private:

	/********************************************/
	//	拍計測用クラス
	/********************************************/
	class Beat
	{
	public:
		//------------------------------------------------------
		//	初期化
		//------------------------------------------------------
		Beat(){ Reset(); }
		void Reset()
		{
			// 時間初期化
			m_iCurrentBeatMSecond = m_iCurrentPlayTime = 0;

			// 仮でBPM60
			SetBPM(60);
		}

		//------------------------------------------------------
		//	毎フレーム呼び出し、現在の再生時間(ミリ秒)を設定する
		//------------------------------------------------------
		void SetPlayTime(int iPlayTime)
		{
			const int iDelta(iPlayTime - m_iCurrentPlayTime);

			// もしかしたら終端かもしれないので止める
			if (iDelta < 0)
			{
				return;
			}
			m_iCurrentPlayTime = iPlayTime;
			m_iCurrentBeatMSecond += iDelta;
		}

		//------------------------------------------------------
		//	BPM変更時に呼び出す
		//------------------------------------------------------
		void SetBPM(float fBPM)
		{
			// 現在の拍計測ミリ秒を初期化
			m_iCurrentBeatMSecond = 0;

			// BPMの値から、何ミリ秒に1拍か計算する
			m_iBeatMSecond = (int)(1000 * (60 / fBPM));
		}

		//------------------------------------------------------
		//	拍の割合(0～1)取得
		//------------------------------------------------------
		float GetPercent()
		{
			while (m_iCurrentBeatMSecond > m_iBeatMSecond) m_iCurrentBeatMSecond -= m_iBeatMSecond;
			return 1 - (float)m_iCurrentBeatMSecond / m_iBeatMSecond; 
		}

	private:
		int m_iCurrentPlayTime;		// 現在の再生カーソル(デルタタイムの計測で使う)
		int m_iCurrentBeatMSecond;	// 現在の()
		int m_iBeatMSecond;		// 何ミリ秒毎に拍を打つか
	}m_tagBeat;


	//------------------------------------------------------
	//	メンバ変数
	//------------------------------------------------------
	std::unique_ptr<tdn2DObj> m_pImage;	// バーの画像
};
