package com.osshare.player.base;

import android.os.Bundle;
import androidx.annotation.Nullable;

/**
 * @author imkk
 * @date 2018/9/20.
 */

public class AbsActivity extends BaseActivity {
    protected int mFlag = 0;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        immersiveStatusBar();
    }

    protected void clearFlag(int flag) {
        mFlag &= ~flag;
    }

    protected boolean hasFlag(int flag) {
        return (mFlag & flag) != 0;
    }
}
