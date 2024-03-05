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

const routes: Routes = [
  {path:"admin/create-poll",component:AdminCreatepollComponent},
  {path:'voter/submit-vote/:identifier',component: VoterSubmitvoteComponent},
  {path:'login',component: LoginComponent},
  {path:'admin-login',component: AdminLoginComponent},
  {path:'register',component: RegisterComponent},
  {path:'admin-register',component: AdminRegisterComponent},
  {path:'voter-main',component: VoterMainComponent},
  {path:'index',component: LandingPageComponent},
  {path:'',component: LandingPageComponent},

];

@NgModule({
  imports: [RouterModule.forRoot(routes)],
  exports: [RouterModule]
})
export class AppRoutingModule { }
