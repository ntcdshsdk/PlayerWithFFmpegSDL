package com.osshare.player.base

import android.content.pm.PackageManager
import android.graphics.Color
import android.os.Build
import android.os.Bundle
import android.view.View
import android.view.Window
import android.view.WindowManager
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import java.util.ArrayList
import java.util.HashMap

open class BaseActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        requestWindowFeature(Window.FEATURE_NO_TITLE)
        //设置使用TransitionManager进行动画，不设置的话系统会使用一个默认的TransitionManager
        window.requestFeature(Window.FEATURE_CONTENT_TRANSITIONS)
        //设置允许通过ActivityOptions.makeSceneTransitionAnimation发送或者接收Bundle
        window.requestFeature(Window.FEATURE_ACTIVITY_TRANSITIONS)
    }

    fun immersiveStatusBar() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            //导致splash页面不正常
            //            getWindow().addFlags(WindowManager.LayoutParams.FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS);
            //            getWindow().addFlags(WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS);
            window.statusBarColor = Color.TRANSPARENT
            window.decorView.systemUiVisibility = View.SYSTEM_UI_FLAG_LAYOUT_STABLE or View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
        } else if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
            window.addFlags(WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS)
        }
    }

    /**
     * 状态栏变为浅色，文字变成深色
     */
    fun setLightStatusBar(light: Boolean) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            val decorView = window.decorView
            if (decorView != null) {
                var vis = decorView.systemUiVisibility
                if (light) {
                    window.addFlags(WindowManager.LayoutParams.FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS)
                    vis = vis or View.SYSTEM_UI_FLAG_LIGHT_STATUS_BAR
                } else {
                    vis = vis and View.SYSTEM_UI_FLAG_LIGHT_STATUS_BAR.inv()
                }
                decorView.systemUiVisibility = vis
            }
        }
    }

    fun immersiveView(view: View?) {
        view?.setPadding(0, view.paddingTop + getStatusBarHeight(), 0, 0)
    }

    fun immersiveView(resId: Int) {
        var view: View = findViewById(resId)
        immersiveView(view)
    }

    fun getStatusBarHeight(): Int {
        return AppUtils.getStatusBarHeight(this)
    }

    fun onBackClick(v: View) {
        ActivityCompat.finishAfterTransition(this)
    }

    protected fun requestPermission(permissions: Array<String>) {
        val unGranters = ArrayList<String>()
        for (permission in permissions) {
            if (ActivityCompat.checkSelfPermission(this, permission) != PackageManager.PERMISSION_GRANTED) {
                unGranters.add(permission)
            }
            //            getPackageManager().isPermissionRevokedByPolicy(permission, getPackageName());
        }
        if (unGranters.size > 0) {
            ActivityCompat.requestPermissions(this, unGranters.toTypedArray(), AppConfig.Code.REQUEST_PERMISSIONS)
        } else {
            onAllPermissionsGranted()
        }
    }

    override fun onRequestPermissionsResult(requestCode: Int, permissions: Array<String>, grantResults: IntArray) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        when (requestCode) {
            AppConfig.Code.REQUEST_PERMISSIONS -> {
                val granters = ArrayList<String>()
                val unGranters = HashMap<String, Boolean>()
                for (i in permissions.indices) {
                    if (grantResults[i] == PackageManager.PERMISSION_GRANTED) {
                        granters.add(permissions[i])
                    } else {
                        var isShould = false
                        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                            isShould = shouldShowRequestPermissionRationale(permissions[i])
                        }
                        unGranters[permissions[i]] = isShould
                    }
                }
                if (unGranters.size > 0) {
                    onPermissionsDenied(unGranters)
                } else {
                    onAllPermissionsGranted()
                }

                if (granters.size > 0) {
                    onPermissionsGranted(granters)
                }
            }
            else -> {
            }
        }
    }

    protected open fun onPermissionsGranted(permissions: List<String>) {

    }

    protected open fun onPermissionsDenied(permissions: Map<String, Boolean>) {

    }

    protected open fun onAllPermissionsGranted() {

    }

}