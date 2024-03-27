import { Component } from '@angular/core';
import { Router } from '@angular/router';
import { VotingService } from '../voting.service';

@Component({
  selector: 'app-voter-main',
  templateUrl: './voter-main.component.html',
  styleUrls: ['./voter-main.component.css']
})
export class VoterMainComponent {

  polls:any;
  timeLeft:number[] = [];
  days:number[] = [];
  hours:number[] = [];
  minutes:number[] = [];
  seconds:number[] = [];
  interval:any;
  active:boolean=true;
  title:string = "";

  constructor(private router:Router, private votingservice:VotingService){}

  ngOnInit(): void {
    this.votingservice.getActivePolls().subscribe((resp:any)=>{
      this.polls = resp.polls;
      this.title = "Izbori u toku";
      resp.polls.forEach((element:any) => {
        console.log(element);
        let time = Math.floor((Date.parse(element.until) - Date.now())/1000);
        this.timeLeft.push(time);
        this.days.push(Math.floor(time/(60*60*24)));
        this.hours.push(Math.floor(time%(60*60*24)/(60*60)));
        this.minutes.push(Math.floor(time%(60*60)/(60)));
        this.seconds.push(time%(60));
        
      });
      this.interval = setInterval(()=>{
        for(let i = 0; i < this.timeLeft.length;i++){
          this.timeLeft[i]--;
          this.days[i] = Math.floor(this.timeLeft[i]/(60*60*24));
          this.hours[i] = Math.floor(this.timeLeft[i]%(60*60*24)/(60*60));
          this.minutes[i] = Math.floor(this.timeLeft[i]%(60*60)/(60));
          this.seconds[i] = this.timeLeft[i]%(60);
        }
      },1000);
    })
  }

  getActivePolls(): void {
    this.votingservice.getActivePolls().subscribe((resp:any)=>{
      this.active = true;
      this.title = "Izbori u toku";
      this.polls = resp.polls;
      resp.polls.forEach((element:any) => {
        let time = Math.floor((Date.parse(element.until) - Date.now())/1000);
        this.timeLeft.push(time);
        this.days.push(Math.floor(time/(60*60*24)));
        this.hours.push(Math.floor(time%(60*60*24)/(60*60)));
        this.minutes.push(Math.floor(time%(60*60)/(60)));
        this.seconds.push(time%(60));
      });
    })
  }

  getFinishedPolls(): void {
    
    this.votingservice.getFinishedPolls().subscribe((resp:any)=>{
      this.title = "Prošli izbori";
      this.active = false;
      this.timeLeft = [];
      this.days = [];
      this.hours = [];
      this.minutes=[];
      this.seconds = [];
      this.polls = resp.polls;
    })
  }
}
