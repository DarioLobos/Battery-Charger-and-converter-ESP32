package com.example.dc_acconverterandcontrolremote
import android.app.Application
import com.example.dc_acconverterandcontrolremote.DatabaseContainer
import com.example.dc_acconverterandcontrolremote.AppDatabaseContainer

class DatabaseApplication: Application() {

    lateinit var databaseContainer: DatabaseContainer

    override fun onCreate() {
        super.onCreate()
        databaseContainer = AppDatabaseContainer(this)

    }


}