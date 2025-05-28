import { COMMA, ENTER } from '@angular/cdk/keycodes';
import { signal } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { ChangeDetectionStrategy, Component } from '@angular/core';
import { FormControl, FormGroup } from '@angular/forms';
import { MatChipEditedEvent, MatChipInputEvent } from '@angular/material/chips';
import { provideNativeDateAdapter } from '@angular/material/core';

const today = new Date();

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrl: './app.component.css',
  standalone: false,
  providers: [provideNativeDateAdapter()],
  changeDetection: ChangeDetectionStrategy.OnPush,
})
export class AppComponent {
  readonly range = new FormGroup({
    fromDate: new FormControl(today),
    fromTime: new FormControl(today),
    toDate: new FormControl(today),
    toTime: new FormControl(today),
  });

  readonly addOnBlur = true;
  readonly separatorKeysCodes = [ENTER, COMMA] as const;
  tags = signal<string[]>([]);
  tokens = signal<string[]>([]);

  constructor(private http: HttpClient) {}

  title = 'ai.chat.hosts.api.moderator.proxy';

  add(event: MatChipInputEvent): void {
    const value = (event.value || '').trim();

    if (value) {
      this.tags.update(tags => [...tags, value]);
    }

    event.chipInput!.clear();
  }
  remove(tag: string): void {
    this.tags.update(tags => {
      const index = tags.indexOf(tag);
      if (index < 0) {
        return tags;
      }

      tags.splice(index, 1);
      return [...tags];
    });
  }

  edit(tag: string, event: MatChipEditedEvent): void {
    const value = event.value.trim();

    if (!value) {
      this.remove(tag);
      return;
    }

    this.tags.update(tags => {
      const index = tags.indexOf(tag);
      if (index >= 0) {
        tags[index] = value;
        return [...tags];
      }
      return tags;
    });
  }

  find(): void {
    this.tokens.set([]);

    let from = this.range.controls.fromDate.value ?? today;
    let fromTime = this.range.controls.fromTime.value ?? today;
    from.setHours(fromTime.getHours());
    from.setMinutes(fromTime.getMinutes());

    let to = this.range.controls.toDate.value ?? today;
    let toTime = this.range.controls.toTime.value ?? today;
    to.setHours(toTime.getHours());
    to.setMinutes(toTime.getMinutes());

    let args = `${this.format(from)} ${this.format(to)}`;
    for (let tag of this.tags()) {
      args += ` ${tag}`;
    }
    this.http.get(`/commands/find/execute?args=${args}`).subscribe(
      (keys) => {
        for (let key of keys as string[]) {
          this.http.get(`/commands/get/execute?args=${key}`).subscribe(
            (keyTokens) => {
              this.tokens.update((tokens) => {
                tokens.push(...keyTokens as string[]);
                return [...tokens];
              })
            },
            (error) => {
              console.error(error);
            });
        }
      },
      (error) => {
        console.error(error);
      }
    )
  }

  private format(date: Date): string {
    let month = (date.getUTCMonth() + 1).toString().padStart(2, '0');
    let day = date.getUTCDate().toString().padStart(2, '0');
    let hour = date.getUTCHours().toString().padStart(2, '0');
    let minute = date.getUTCMinutes().toString().padStart(2, '0');
    return `${date.getUTCFullYear()}${month}${day}${hour}${minute}000000000`;
  }
}
