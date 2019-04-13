package com.osshare.player.base;

import android.content.Context;
import android.content.res.Resources;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;

public class AppUtils {
    public static int STATUS_BAR_HEIGHT;
    public static int getStatusBarHeight(Context context) {
        if (STATUS_BAR_HEIGHT <= 0) {
            try {
                Resources res = getResources(context);
                int resourceId = res.getIdentifier("status_bar_height", "dimen", "android");
                if (resourceId > 0) {
                    STATUS_BAR_HEIGHT = res.getDimensionPixelSize(resourceId);
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
        return STATUS_BAR_HEIGHT;
    }

    private static Resources getResources(Context context) {
        if (context == null) {
            return Resources.getSystem();
        }
        Resources res = context.getResources();
        return res == null ? Resources.getSystem() : res;
    }

    public static String readRaw(Context context, int resId) {
        StringBuilder builder = new StringBuilder();
        try {
            InputStream inStream = getResources(context).openRawResource(resId);
            BufferedReader reader = new BufferedReader(new InputStreamReader(inStream));
            String lineText;
            while ((lineText = reader.readLine()) != null) {
                builder.append(lineText).append('\n');
            }
        } catch (IOException e) {
            throw new RuntimeException("Could not open Raw resource:" + resId, e);
        } catch (Resources.NotFoundException nfe) {
            throw new RuntimeException("Raw resource " + resId + " not found", nfe);
        }

        return builder.toString();
    }
}
