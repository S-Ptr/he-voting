import { Component, OnInit } from '@angular/core';
import { UserService } from '../user.service';
import { Router } from '@angular/router';
import { MatInputModule } from '@angular/material/input';

@Component({
  selector: 'app-login',
  templateUrl: './login.component.html',
  styleUrls: ['./login.component.css']
})
export class LoginComponent implements OnInit {

  constructor(private router: Router, private userservice: UserService) { }

  ngOnInit(): void {
  }

  username: string = ""
  pass: string = ""
  msg: string = ""
  hide:boolean = true;


  PrijaviSe() {

    let data = {
      username:this.username,
      pass:this.pass
    }

    this.userservice.login(data).subscribe((data:any) => {
      console.log(data)
      if(data.error){
        this.msg = data.error;
      }
      if (data) {
        let user = {name:data.name, type:"voter"}
        localStorage.setItem('user', JSON.stringify(user));
        this.router.navigate(['/voter-main']);
      }
      else {
        this.msg = 'Neispravno uneti podaci';
      }
    })


  }

}
