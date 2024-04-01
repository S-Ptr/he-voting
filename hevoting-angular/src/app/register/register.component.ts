import { Component, OnInit } from '@angular/core';
import { UserService } from '../user.service';
import { Router } from '@angular/router';

@Component({
  selector: 'app-register',
  templateUrl: './register.component.html',
  styleUrls: ['./register.component.css']
})
export class RegisterComponent {

  constructor(private router: Router, private userservice: UserService) { }

  ngOnInit(): void {
  }

  username: string = ""
  pass: string = ""
  msg: string = ""


  register() {
    if(this.username == '' || this.pass == ''){
      this.msg == 'Please fill out all fields'
    }

    let data = {
      username:this.username,
      pass:this.pass
    }

    this.userservice.register(data).subscribe((data:any) => {
      if(data == null){
        this.router.navigate(['/login']);
        return;
      }
      if(data.error){
        this.msg = data.error;
      }else{
        this.router.navigate(['/login']);
      }
    })


  }

}
