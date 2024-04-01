import { NgModule } from '@angular/core';
import { RouterModule, Routes } from '@angular/router';
import { AdminCreatepollComponent } from './admin-createpoll/admin-createpoll.component';
import { VoterSubmitvoteComponent } from './voter-submitvote/voter-submitvote.component';
import { LoginComponent } from './login/login.component';
import { AdminLoginComponent } from './admin-login/admin-login.component';
import { VoterMainComponent } from './voter-main/voter-main.component';
import { RegisterComponent } from './register/register.component';
import { AdminRegisterComponent } from './admin-register/admin-register.component';
import { LandingPageComponent } from './landing-page/landing-page.component';

import { AdminAuthGuard } from './admin-auth.guard';
import { UserAuthGuard } from './user-auth.guard';
import { LogoutComponent } from './logout/logout.component';

const routes: Routes = [
  {path:'index',component: LandingPageComponent},
  {path:'',component: LandingPageComponent},
  {path:'logout',component: LogoutComponent},
  {path:'login',component: LoginComponent},
  {path:'admin-login',component: AdminLoginComponent},
  {path:'register',component: RegisterComponent},
  {path:'admin-register',component: AdminRegisterComponent},
  {path:"admin/create-poll",component:AdminCreatepollComponent, canActivate:[AdminAuthGuard]},
  {path:'voter/poll/:identifier',component: VoterSubmitvoteComponent, canActivate:[UserAuthGuard]},
  {path:'voter-main',component: VoterMainComponent, canActivate:[UserAuthGuard]},
];

@NgModule({
  imports: [RouterModule.forRoot(routes)],
  exports: [RouterModule]
})
export class AppRoutingModule { }
