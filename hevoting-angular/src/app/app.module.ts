import { NgModule } from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';

import { AppRoutingModule } from './app-routing.module';
import { AppComponent } from './app.component';
import { VoterSubmitvoteComponent } from './voter-submitvote/voter-submitvote.component';
import { LoginComponent } from './login/login.component';
import { AdminCreatepollComponent } from './admin-createpoll/admin-createpoll.component';
import {ReactiveFormsModule} from '@angular/forms';

import { FormsModule } from '@angular/forms';
import { HttpClientModule } from '@angular/common/http';
import { UserPollComponent } from './user-poll/user-poll.component';
import { VoterMainComponent } from './voter-main/voter-main.component';
import { AdminLoginComponent } from './admin-login/admin-login.component';
import { RegisterComponent } from './register/register.component';

@NgModule({
  declarations: [
    AppComponent,
    VoterSubmitvoteComponent,
    LoginComponent,
    AdminCreatepollComponent,
    UserPollComponent,
    VoterMainComponent,
    AdminLoginComponent,
    RegisterComponent
  ],
  imports: [
    BrowserModule,
    AppRoutingModule,
    FormsModule,
    ReactiveFormsModule,
    HttpClientModule
  ],
  providers: [],
  bootstrap: [AppComponent]
})
export class AppModule { }
